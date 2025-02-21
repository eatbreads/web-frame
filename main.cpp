#include <iostream>
using namespace std;

#include <web/server.h>
using namespace yazi::web;

void hello(const Request & req, Response & resp)// 处理 /hello 路径的 GET 请求
{
    string name = req.get("name");
    string age = req.get("age");
    string host = req.user_host();
    
    resp.data(Response::HTML, "hello, " + name + "," + age + "," + host);
    
}

void reply(const Request & req, Response & resp)// 处理 /reply 路径的 POST 请求
{
    Json name = req.post("name");
    Json age = req.post("age");

    if (name.is_null())
    {
        // 姓名为空
    }
    
    Json json;
    json["name"] = name;
    json["age"] = age;
    resp.data(Response::JSON, json.str());
}

int main()
{
    auto server = Singleton<Server>::instance();// 获取 Server 单例
    server->bind("/hello", hello);              // 绑定 /hello 路径到 hello 函数
    server->bind("/reply", reply);              // 绑定 /reply 路径到 reply 函数
    server->start();
    return 0;
}
