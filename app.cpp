#include "router.hpp"
#include "iostream"

int main(int argc, char const *argv[])
{
    rt::router ros;

    // 注册静态路由
    ros.on("app/index", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params)
           {
        data = "Static Index Page";
        return rt::FLAG_DONE; });

    // 注册动态路由 :id 和 :action
    ros.on("user/:id", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params)
           {
        std::string id = params.count("id") ? params.at("id") : "unknown";
        data = "User Profile: " + id;
        return rt::FLAG_DONE; });

    ros.on("api/:version/data", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params)
           {
        std::string ver = params.count("version") ? params.at("version") : "v1";
        data = "Data from API version: " + ver;
        std::cout << "URL: " << url << " -> " << data << std::endl;
        return rt::FLAG_DONE; });
    ros.on("api/:version/list", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params)
           {
        std::string ver = params.count("version") ? params.at("version") : "v1";
        data = "Data from API version: " + ver;
        std::cout << "URL: " << url << " -> " << data << std::endl;
        return rt::FLAG_DONE; });

    // 测试动态路由
    std::string result_data;
    std::string test_url = "user/12345";

    auto [ptr, params] = ros.get(test_url);

    if (!ptr.expired())
    {
        auto node = ptr.lock();
        node->func(test_url, result_data, params);
        std::cout << "URL: " << test_url << " -> " << result_data << std::endl;

        // 打印提取的参数
        std::cout << "Parameters: ";
        for (const auto &p : params)
        {
            std::cout << "[" << p.first << "=" << p.second << "] ";
        }
        std::cout << std::endl;
    }
    else
    {
        std::cout << "Route not found: " << test_url << std::endl;
    }

    // 测试另一个动态路由
    test_url = "api/v2/data";
    auto [ptr2, params2] = ros.get(test_url);
    if (!ptr2.expired())
    {
        auto node = ptr2.lock();
        node->func(test_url, result_data, params2);
    }
    test_url = "api/v2/list";
    auto [ptr3, params3] = ros.get(test_url);
    if (!ptr3.expired())
    {
        auto node = ptr3.lock();
        node->func(test_url, result_data, params3);
    }

    return 0;
}