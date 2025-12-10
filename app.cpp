#include "router.hpp"
#include "iostream"

int main(int argc, char const *argv[])
{
    rt::router ros;

    ros.on("app/index");

    auto ptr1 = ros.get("app/index/");
    std::cout << (ptr1.lock())->name << std::endl;

    auto ptr2 = ros.get("app/index");
    std::cout << (ptr2.lock())->name << std::endl;

    auto ptr3 = ros.get("app");
    std::cout << (ptr3.lock())->name << std::endl;

    auto ptr4 = ros.get("app/");
    std::cout << (ptr4.lock())->name << std::endl;

    auto ptr5 = ros.get("/app");
    std::cout << (ptr5.lock())->name << std::endl;

    auto ptr6 = ros.get("/");
    std::cout << (ptr6.lock())->name << std::endl;

    auto ptr7 = ros.get("");
    std::cout << (ptr7.lock())->name << std::endl;

    auto ptr8 = ros.get("//////////");
    std::cout << (ptr8.lock())->name << std::endl;

    auto ptr9 = ros.get("app///////////index/");
    std::cout << (ptr9.lock())->name << std::endl;
    return 0;
}
