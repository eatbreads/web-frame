#pragma once

namespace yazi
{
    namespace utility
    {
        template <typename T>
        class Singleton
        {
        public:
            static T * instance()
            {
                static T instance;
                return &instance;
            }

        private:
            Singleton() = default;
            Singleton(const Singleton<T> &) = delete;
            Singleton<T> & operator = (const Singleton<T> &) = delete;
            ~Singleton() = default;
        };
        //这边有个宏定义,使得他支持这样在类的定义的时候加上这个就实现单例了
        //所以那些单独写一个宏的操作应该都是转化成了一些函数或者一些类(友元类如下)
        #define SINGLETON(classname)                                    \
            friend class Singleton<classname>;                          \
            private:                                                    \
                classname() = default;                                  \
                classname(const classname &) = delete;                  \
                classname & operator = (const classname &) = delete;    \
                ~classname() = default
    }
}