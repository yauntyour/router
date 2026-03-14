#include "router.hpp"
#include "iostream"

int main(int argc, char const *argv[])
{
    rt::router ros;

    ros.on("app/index");
    ros.on("/", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params) -> int
           {
        std::cout << "root" << std::endl;
        return 0; });

    std::string url = "/";
    std::string data = "";
    auto [ptr, param] = ros.get("/");
    auto node = ptr.lock();
    node->func(url, data, param);

    auto [ptr1, param1] = ros.get("app/index/");
    std::cout << (ptr1.lock())->name << std::endl;

    auto [ptr2, param2] = ros.get("app/index");
    std::cout << (ptr2.lock())->name << std::endl;

    auto [ptr3, param3] = ros.get("app");
    std::cout << (ptr3.lock())->name << std::endl;

    auto [ptr4, param4] = ros.get("app/");
    std::cout << (ptr4.lock())->name << std::endl;

    auto [ptr5, param5] = ros.get("/app");
    std::cout << (ptr5.lock())->name << std::endl;

    auto [ptr6, param6] = ros.get("/");
    std::cout << (ptr6.lock())->name << std::endl;

    auto [ptr7, param7] = ros.get("");
    std::cout << (ptr7.lock())->name << std::endl;

    auto [ptr8, param8] = ros.get("//////////");
    std::cout << (ptr8.lock())->name << std::endl;

    auto [ptr9, param9] = ros.get("app///////////index/");
    std::cout << (ptr9.lock())->name << std::endl;

    auto [ptr10, param10] = ros.get("app/api");
    if (!ptr10.expired())
    {
        std::cout << (ptr10.lock())->name << std::endl;
    }
    else
    {
        std::cout << "not found" << std::endl;
    }
    return 0;
}