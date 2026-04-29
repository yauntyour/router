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

## 回调函数类型

```c++
// 标准路由回调：参数 (url, data输出, params动态路由参数字典) -> 状态码
using HandlerFunc = std::function<int(std::string &, std::string &, const std::map<std::string, std::string> &)>;

//example:
int default_func(std::string &url, std::string &data, const std::map<std::string, std::string> &params)
{
    data = "<null>";
    return FLAG_DONE;
}
```

## 状态码：（对路由无影响，表明回调函数的执行情况，可以自行安排）

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
int page_func(std::string &url, std::string &data, const std::map<std::string, std::string> &params)
{
    data = "<h1>Hello</h1>";
    return FLAG_DONE;
}

ros.on("app/index");
ros.on("app/test");
ros.on("app/index/page", page_func);
```

对应的节点为：

```text
app/
  index/
    page/
  test/
```

不提供回调函数则默认设置为 `default_func`

## 动态路由

支持 `:param` 格式的动态路径参数匹配。

```c++
// 注册
ros.on("user/:id", [](std::string &url, std::string &data, const std::map<std::string, std::string> &params) {
    std::string id = params.at("id");
    data = "User Profile: " + id;
    return rt::FLAG_DONE;
});

// 匹配
auto [ptr, params] = ros.get("user/12345");
// params["id"] == "12345"
```

## 流式路由（Stream Router）

支持通过回调分块输出数据的流式路由，适用于 SSE、大文件传输、实时数据推送等场景。

### 类型定义

```c++
using WriteCallback = std::function<void(const std::string &)>;
using StreamHandlerFunc = std::function<void(std::string &, WriteCallback, const std::map<std::string, std::string> &)>;
```

- `WriteCallback` — 每次调用写入一个数据块
- `StreamHandlerFunc` — 处理器通过反复调用 `WriteCallback` 输出数据

### 注册 & 调用

```c++
// 注册流式路由
ros.on_stream("stream/chat", [](std::string &req, rt::WriteCallback write, const std::map<std::string, std::string> &params) {
    write("[start]");
    write("Hello");
    write(", ");
    write("World");
    write("[end]");
});

// 检查是否存在
if (ros.has_stream_handler("stream/chat")) {
    // ...
}

// 获取并调用
auto handler = ros.get_stream_handler("stream/chat");
if (handler) {
    std::string req = "ping";
    std::map<std::string, std::string> params;
    handler(req, [](const std::string &chunk) {
        std::cout << chunk;  // 输出: [start]Hello, World[end]
    }, params);
}
```
