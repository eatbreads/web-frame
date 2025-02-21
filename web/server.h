#pragma once

#include <fstream>
#include <sstream>
#include <functional>
#include <string>
using std::string;

#include <frame/server.h>

#include <web/request.h>
#include <web/response.h>

namespace yazi
{
    namespace web   
    {
        typedef std::function<void(const Request &, Response &)> server_handler;

        class Server
        {
            SINGLETON(Server);
        public:
            void start();           //所以说web的server封装了frame的server,并且在start里面调用frame的start来初始化,内部已经弄好一堆东西了,这边直接调用即可

            void bind(const string & path, server_handler handler);// 绑定 URL 路径和处理函数
            string handle(const Request & req);// 处理 HTTP 请求并返回响应

            void set_template_folder(const string & template_folder);// 设置模板文件夹,好像是用于返回那种资源文件,比如已经写好的js代码
            string get_template_folder() const;// 获取模板文件夹

            void set_static_folder(const string & static_folder);// 
            string get_static_folder() const;// 这边两个get都是获取的是绝对路径,所以要调用system的get_root来拼接

        private:
            std::map<string, server_handler> m_handlers;// 存储 URL 路径和处理函数的映射
            string m_template_folder = "template";// 模板文件夹,这里俩都是默认的相对路径
            string m_static_folder = "static";// 静态文件夹                            
        };
    }
}
