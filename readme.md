# 原生C++实现的路由器：Router

## Header-only & 反傻逼设计

```c++
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
```

## 回调函数示例

```c++
std::function<int(std::string &, std::string &)> func;

//example:
int default_func(std::string &url, std::string &data)
{
 data = "<null>";
 return FLAG_DONE;
}
```

## 异常状态标注：（对路由无影响，表明回调函数的执行情况，可以自行安排）

```c++
enum service_state
{
 FLAG_DONE = 0,
 FLAG_ERROR,
    FLAG_WARN
};
```

## 基本语法

全自动注册节点（基于哈希表）

```c++
int page_func(std::string &url, std::string &data)
{
 data = "<h1>Hello</h1>";
 return FLAG_DONE;
}

ros.on("app/index");
ros.on("app/test");
ros.on("app/index/page",page_func);
```

对应的节点为：

```
app/
 index/
  page/
 test/
```

不提供回调函数则默认设置为`default_func`

## 动态路由

```c++
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
```
