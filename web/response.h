#pragma once

#include <fstream>
#include <sstream>
#include <string>
using std::string;

namespace yazi
{
    namespace web
    {
        class Response
        {
        public:
            enum Type
            {
                HTML = 0,
                JSON,
                JS,
                CSS,
                JPG,
                PNG,
                GIF,
                ICO
            };

            Response();
            ~Response(); 

            void code(int code);        //响应码
            void data(Type type, const string &data);   //响应数据的注入
            string data() const;                        //这个无参数的data是用来返回值的(他也确实有返回值),其他的是用来注入的

            void html(const string & data);         //这里两个是直接通过传入的data来初始化m_data
            void json(const string & data);         //类型不同只是为了在响应报文data()里面发送不同的文件格式而已,m_data体没有区别

            static string page_not_found();         //界面未找到返回这个
            void render(const string & filename);   //用于给客户端发送html等代码进行页面渲染

        private:
            int m_code;
            Type m_type;
            string m_data;
        };
    }
}
