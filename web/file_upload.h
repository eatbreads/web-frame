#pragma once

#include <map>
#include <cstring>
#include <string>
using std::string;

namespace yazi
{
    namespace web
    {
        class FileUpload    //上传文件类
        {
        public:
            FileUpload(); 
            ~FileUpload();

            string name() const;        //名字
            string filename() const;    //可能是原始名字,对方收到的是名字?
            string type() const; 
            const char * data() const;
            int size() const; 
            string extension() const;   //返回上传文件的扩展名

            void parse(const char * buf, int len);  //解析http请求来提取上传信息,这里只是解析了他内部的头,然后把底下成员数据都赋值好了,
            bool save(const string & filename);     //根据parse解析出来的信息,把数据存放进文件里面

        private:
            string m_name;  
            string m_type;
            string m_filename;//文件名
            string m_extension;//扩展名
            string m_boundary;//边界(是一串编码)
            std::map<string, string> m_headers;//存放消息对     ps:其实没怎么用到
            const char * m_buf;
            int m_len;
            int m_start;    //二进制文件开始的位置   
            int m_end;
        };
    }
}
