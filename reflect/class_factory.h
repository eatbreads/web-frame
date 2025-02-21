#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>

#include <utility/singleton.h>
using namespace yazi::utility;

#include <reflect/class_field.h>
#include <reflect/class_method.h>

namespace yazi
{
    namespace reflect
    {
        class Object    //所有要支持反射的类都要继承于这个类,并且调用宏
        {               //,因为我们并不知道传入的字符串是什么,所以要统一用基类指针
        public:
            Object() = default;
            virtual ~Object() = default;

            void set_class_name(const string & class_name);
            const string & get_class_name() const;

            int get_field_count();              //这里其实都是调用工厂函数提供的那些接口(那一堆get)
            ClassField * get_field(int pos);
            ClassField * get_field(const string & field_name);

            template <typename T>
            void get(const string & field_name, T & value);     //这边是两个模板参数,支持对自己写的类A(继承于Object的)
                                                                //拿到对应的字段,get把值注入value,set写入类里面,使用模板效果类似宏
            template <typename T>
            void set(const string & field_name, const T & value);

            template <typename R = void, typename ...Args>
            R call(const string & method_name, Args&&... args);     //这边是可变参数的万能引用,提高兼容性

        private:
            string m_class_name;            //这个类的名字
        };

        typedef Object * (*create_object)();    //等价于using create_object=Object*(*)(void)
                                                //表示一个返回函数指针的的函数指针
        class ClassFactory
        {
            SINGLETON(ClassFactory);            //注册单例类
        public:
            // reflect class,反射类
            void register_class(const string & class_name, create_object func);     //就是加入map
            Object * create_class(const string & class_name);                   //就是返回second,没找到返回nullptr

            // reflect class field,反射数据成员,这边就是把类名和类中字段的信息放进来注册,就可以单独注册类中的字段
            void register_class_field(const string & class_name, const string & field_name, const string & field_type, size_t offset);
            int get_class_field_count(const string & class_name);               //这里就是返回已经被注册的类的字段数量
            ClassField * get_class_field(const string & class_name, int pos);
            ClassField * get_class_field(const string & class_name, const string & field_name);     //拿到具体的字段

            // reflect class method,反射成员函数,这边就是和上面差不多了,也是注册信息,第三参是函数指针(会变成unsign long)
            void register_class_method(const string & class_name, const string &method_name, uintptr_t method);
            int get_class_method_count(const string & class_name);
            ClassMethod * get_class_method(const string & class_name, int pos);
            ClassMethod * get_class_method(const string & class_name, const string & method_name);

        private:
            std::map<string, create_object> m_class_map;                    //这里就是对象名字和生产类的函数的映射
            std::map<string, std::vector<ClassField *>> m_class_fields;     //这边也是这map,second是类字段,即每个类对应的字段
            std::map<string, std::vector<ClassMethod *>> m_class_methods;   //这里第二参数存放的是类成员函数指针,这边用ClassMethod来绑定(封装的成员函数类)
        };                      //ps:用function绑定的时候签名的参数的第一参是this指针,需要写出来,如function<int(object*)>fun

        template <typename T>
        void Object::get(const string & field_name, T & value)          //get具体实现,本质上是调用了工厂的拿到具体字段的方法
        {
            ClassField * field = Singleton<ClassFactory>::instance()->get_class_field(m_class_name, field_name);
            if (field == nullptr)
            {
                return;
            }
            size_t offset = field->offset();            //返回内部的偏移量信息(在注册反射字段的时候偏移量就已经被写入了)
            value = *((T *)((unsigned char *)(this) + offset));             //这里是黑科技,先强转化成char(一字节),就支持指针移动,然后直接强转成要的类型即可
        }       

        template <typename T>
        void Object::set(const string & field_name, const T & value)
        {
            ClassField * field = Singleton<ClassFactory>::instance()->get_class_field(m_class_name, field_name);
            if (field == nullptr)
            {
                return;
            }
            size_t offset = field->offset();
            *((T *)((unsigned char *)(this) + offset)) = value; //实现同理,就是反过来而已
        }

        template <typename R, typename ...Args>     //这里的R作为返回值是模板,所以调用call的时候一定要<>指定第一个R是什么,不然就不知道返回什么
        R Object::call(const string & method_name, Args&&... args)
        {
            ClassFactory * factory = Singleton<ClassFactory>::instance();
            ClassMethod * method = factory->get_class_method(m_class_name, method_name);
            if (method == nullptr)
            {
                return R();
            }
            auto func = method->method();
            typedef std::function<R(decltype(this), Args...)> class_method;         //这里也是巨黑科技,用到了decltype和这种指针的强转和解引用
            return (*((class_method *)func))(this, args...);                                            
        }
    }
}













