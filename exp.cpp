#include "router.hpp"
#include "iostream"
#include "thread"
#include "chrono"

int main(int argc, char const *argv[])
{
    rt::router ros;

    // ======== 1. 静态路由 ========
    std::cout << "=== Static Route ===" << std::endl;
    ros.on("app/index", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params)
           {
        data = "Static Index Page";
        return rt::FLAG_DONE; });

    auto [ptr, param] = ros.get("app/index");
    if (!ptr.expired())
    {
        std::string url = "app/index", data;
        ptr.lock()->func(url, data, param);
        std::cout << "GET /app/index -> " << data << std::endl;
    }

    // ======== 2. 动态路由 ========
    std::cout << "\n=== Dynamic Route ===" << std::endl;
    ros.on("user/:id", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params)
           {
        std::string id = params.count("id") ? params.at("id") : "unknown";
        data = "User Profile: " + id;
        return rt::FLAG_DONE; });

    ros.on("api/:version/data", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params)
           {
        std::string ver = params.count("version") ? params.at("version") : "v1";
        data = "Data from API version: " + ver;
        return rt::FLAG_DONE; });

    std::string result_data;
    std::string test_url = "user/12345";
    auto [ptr2, params2] = ros.get(test_url);
    ptr2.lock()->func(test_url, result_data, params2);
    std::cout << "GET /user/12345 -> " << result_data << std::endl;

    test_url = "api/v2/data";
    auto [ptr3, params3] = ros.get(test_url);
    ptr3.lock()->func(test_url, result_data, params3);
    std::cout << "GET /api/v2/data -> " << result_data << std::endl;

    // ======== 3. 流式路由 (新增) ========
    std::cout << "\n=== Stream Route (New) ===" << std::endl;

    // 注册流式路由
    ros.on_stream("stream/chat", [](std::string &req, rt::WriteCallback write, const std::map<std::string, std::string> &params)
           {
        write("[start]");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        write("Hello");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        write(", ");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        write("World");
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        write("[end]"); });

    ros.on_stream("stream/data", [](std::string &req, rt::WriteCallback write, const std::map<std::string, std::string> &params)
           {
        for (int i = 1; i <= 3; i++)
        {
            write("{ \"chunk\": " + std::to_string(i) + " }");
        } });

    // 检查流式路由是否存在
    if (ros.has_stream_handler("stream/chat"))
    {
        std::cout << "Found stream handler: stream/chat" << std::endl;
    }

    // 调用无参数的流式路由
    auto handler = ros.get_stream_handler("stream/chat");
    if (handler)
    {
        std::cout << "Invoking stream/chat:" << std::endl;
        std::cout << "  ";
        std::string req = "request-data";
        std::map<std::string, std::string> empty_params;
        handler(req, [](const std::string &chunk)
                { std::cout << "[" << chunk << "]"; },
                empty_params);
        std::cout << std::endl;
    }

    // 调用带多次数据块输出的流式路由
    auto handler2 = ros.get_stream_handler("stream/data");
    if (handler2)
    {
        std::cout << "\nInvoking stream/data:" << std::endl;
        std::cout << "  ";
        std::string req = "";
        std::map<std::string, std::string> empty_params;
        handler2(req, [](const std::string &chunk)
                 { std::cout << chunk << " "; },
                 empty_params);
        std::cout << std::endl;
    }

    return 0;
}