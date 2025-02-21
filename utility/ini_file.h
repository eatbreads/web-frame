#pragma once

#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
using std::string;

#include <sstream>
using std::stringstream;

#include <fstream>
using std::ifstream;
using std::ofstream;

#include <utility/value.h>

namespace yazi
{
    namespace utility
    {
        class IniFile           //用于处理配置文件
        {
            typedef std::map<string, Value> Section;        //使用了键值对来存储配置信息
        public:
            IniFile() = default;
            IniFile(const string & filename);           //设置文件名
            ~IniFile() = default;

            bool load(const string & filename);         //载入文件
            void save(const string & filename);
            string str() const;
            void show() const;
            void clear();

            Value & get(const string & section, const string & key);
            void set(const string & section, const string & key, const Value & value);

            bool has(const string & section);
            bool has(const string & section, const string & key);//是否有键值对

            void remove(const string & section);
            void remove(const string & section, const string & key);//删除键值对

            Section & operator [] (const string & section)  //返回映射
            {
                return m_sections[section];
            }

        private:
            string trim(string s);          //用于去除字符串中的空格

        private:
            string m_filename;
            std::map<string, Section> m_sections;
        };
    }
}
