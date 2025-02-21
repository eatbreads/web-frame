#pragma once

#include <string>
using std::string;

namespace yazi
{
    namespace reflect
    {
        class ClassMethod       //这是封装的成员函数类
        {
        public:
            ClassMethod() : m_name(""), m_method(0) {}
            ClassMethod(const string & name, uintptr_t method) : m_name(name), m_method(method) {}
            ~ClassMethod() = default;

            const string & name() const
            {
                return m_name;
            }

            uintptr_t method() const        
            {
                return m_method;
            }

        private:
            string m_name;
            uintptr_t m_method;         //使用这个就可以把函数指针统一化,可以使用一个统一的类型来存储,不然每一个签名不一样就无法统一
        };                              //即使用function也无法统一,他只能统一具有相同函数签名的不同东西,函数,函数对象,lambda这种
    }
}