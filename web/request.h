#pragma once

#include <map>
#include <string>
using std::string;

#include <json/json.h>
using namespace yazi::json;

#include <web/file_upload.h>

namespace yazi
{
    namespace web
    {
        class Request
        {
        public:
            Request() = default;
            ~Request() = default;

            int parse_header(const char * buf, int len);    //解析http头部
            void parse_body(const char * buf, int len);     // 解析 HTTP 正文

            bool is_get() const;// 检查 HTTP 方法是否为 GET
            bool is_post() const;// 检查 HTTP 方法是否为 POST

            string get(const string & name) const;// 获取 GET 参数的值
            Json post(const string & name = "") const; // 获取 POST 参数的值，如果 name 为空，则返回整个 POST 数据作为 JSON 对象
            string header(const string & name) const;// 获取 HTTP 头部的值
            string cookie(const string & name) const;// 获取 Cookie 的值
            FileUpload file(const string & name) const;// 获取上传的文件
            string path() const;// 获取请求的路径
            string user_agent() const;// 获取 User-Agent 头部的值
            string user_host() const;// 获取 User-Host 头部的值

            int content_length();// 获取 HTTP 正文的长度 

            void show() const;// 显示 HTTP 请求的详细信息

        private:
            string m_method;        //请求方法
            string m_uri;           //uri,带问号的路径
            string m_proto;         //协议类型 
            string m_path;          //路径
            string m_body;          //报文体
            string m_query_string;  //uri的问号部分,可能是a=1&b=2&c=3
            std::map<string, string> m_get;    //这个就是key是a,value是1,key是b,value是2这样
            Json m_post;
            std::map<string, string> m_headers; //这边是报文头的键值对
            std::map<string, string> m_cookies; 
            std::map<string, FileUpload> m_files;//这里存放着表单和对应这个类的映射
            
        };
    }
}
