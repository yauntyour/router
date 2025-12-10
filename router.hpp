#pragma once
#ifndef __ROUTER__H__
#define __ROUTER__H__

#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <memory>

namespace rt
{
    enum service_state
    {
        FLAG_DONE = 0,
        FLAG_ERROR,
        FLAG_WARN
    };
    int default_func(std::string &url, std::string &data)
    {
        data = "<null>";
        return FLAG_DONE;
    }

    struct node
    {
        std::string name = "";
        std::function<int(std::string &, std::string &)> func = default_func;
        std::unordered_map<std::string, std::shared_ptr<node>> list;
        node(std::string &name, std::function<int(std::string &, std::string &)> func = default_func) : name(name), func(func) {};
        node() = default;
        ~node()
        {
            list.clear();
        }
    };

    class router
    {
    private:
        std::shared_ptr<node> base = std::shared_ptr<node>(new node());
        std::unordered_map<std::string, std::shared_ptr<node>> list;
        void fix_url(std::string &url)
        {
            if (!url.empty())
            {
                std::string result = "";
                char end = 0;
                for (size_t i = 0; i < url.length(); i++)
                {
                    if (url[i] == '/' && end == '/')
                    {
                        continue;
                    }
                    result += url[i];
                    end = url[i];
                }
                url = result;
                if (url != "/")
                {
                    if (url[0] == '/')
                    {
                        url = url.substr(1, url.length() - 1);
                    }
                    if (url[url.length() - 1] == '/')
                    {
                        url = url.substr(0, url.length() - 1);
                    }
                }
                else
                {
                    url = "";
                }
            }
        }
        void split_on(std::string &str, char d, std::function<void(std::string, size_t)> cb)
        {
            size_t start = 0;
            for (size_t i = 0; i <= str.size(); ++i)
            {
                if (i == str.size() || str[i] == d)
                {
                    cb(str.substr(start, i - start), i);
                    start = i + 1;
                }
            }
        }
        std::weak_ptr<node> split_get(std::string &str, char d, std::function<std::weak_ptr<node>(std::string, size_t)> cb)
        {
            size_t start = 0;
            for (size_t i = 0; i <= str.size(); ++i)
            {
                if (i == str.size() || str[i] == d)
                {
                    auto p = cb(str.substr(start, i - start), i);
                    if (!p.expired())
                    {
                        return p;
                    }
                    start = i + 1;
                }
            }
            return std::weak_ptr<node>();
        }

    public:
        router() = default;
        ~router()
        {
            list.clear();
        };
        void on(std::string url, std::function<int(std::string &, std::string &)> func = default_func)
        {
            fix_url(url);
            if (url == "")
            {
                base->func = func;
                return;
            }
            auto *temp = &list;
            split_on(url, '/', [&](std::string part, size_t i)
                     {
                        auto it = temp->find(part);
                        if (it != temp->end())
                        {
                            if (i == url.length())
                            {
                                if (it->second != nullptr)
                                {
                                    it->second->func = func;
                                }else
                                {
                                    it->second = std::shared_ptr<node>(new node(part,func));
                                }
                            }else
                            {
                                temp = &(temp->operator[](part)->list);
                            }
                        }else
                        {
                            if (i == url.length())
                            {
                                temp->operator[](part) = std::shared_ptr<node>(new node(part,func));
                            }else
                            {
                                temp->operator[](part) = std::shared_ptr<node>(new node(part,default_func));
                            }
                            temp = &(temp->operator[](part)->list);
                        } });
        }
        std::weak_ptr<node> get(std::string url)
        {
            fix_url(url);
            if (url == "")
            {
                return std::weak_ptr<node>(base);
            }
            auto *temp = &list;
            std::weak_ptr<node> ptr = split_get(url, '/', [&](std::string part, size_t i) -> std::weak_ptr<node>
                                                {
                        auto it = temp->find(part);
                        if (it != temp->end())
                        {
                            if (i == url.length())
                            {
                                return std::weak_ptr<node>(it->second);
                            }else{
                                temp = &(temp->operator[](part)->list);
                                return std::weak_ptr<node>();
                            }
                        }else
                        {
                            return std::weak_ptr<node>();
                        } });
            return ptr;
        }
    };
} // namespace rot

#endif //!__ROUTER__H__