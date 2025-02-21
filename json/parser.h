#pragma once

#include <fstream>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <cstdlib>
#include <json/json.h>


namespace yazi
{
    namespace json
    {
        class Parser 
        {
        public:
            Parser() = default;
            ~Parser() = default;

            void load(const string & filename);//就是从一个文件加载json串
            void load(const char * buf, int len);//这个是直接用字符串载入
            Json parse();                       //然后使用这个解析,会返回一个json 

        private:
            void skip_white_space();        //忽略空白符
            char get_next_token();          //读取下一个字符

            Json parse_null();              //解析空值
            Json parse_bool();              //解析bool,下面相同 
            Json parse_number();
            string parse_string();
            Json parse_array();
            Json parse_object();

            bool in_range(int x, int lower, int upper)
            {
                return (x >= lower && x <= upper);
            }

        private:
            string m_str;
            size_t m_idx = 0;
        };
    }
}
