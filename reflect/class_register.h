#pragma once

#include <reflect/class_factory.h>

namespace yazi
{
    namespace reflect
    {
        class ClassRegister     //这是个辅助的类,和class_factory配合的,即在工厂类上面再封装一层,但是实际上我们不会直接见到这个类
        {/////////////////////////////////////////////////////////////////////////////////
        public:
            ClassRegister(const string & class_name, create_object func)
            {
                // register class,就是使用这个类来简介操作工厂,下面这一行就是使用工厂类来执行,把执行函数放入map里面
                Singleton<ClassFactory>::instance()->register_class(class_name, func);
            }

            ClassRegister(const string & class_name, const string & field_name, const string & field_type, uintptr_t offset)
            {
                // register class field,同样调用工厂类的函数进行注册
                Singleton<ClassFactory>::instance()->register_class_field(class_name, field_name, field_type, offset);
            }

            ClassRegister(const string & class_name, const string & method_name, uintptr_t method)
            {
                // register class method
                Singleton<ClassFactory>::instance()->register_class_method(class_name, method_name, method);
            }
        };/////////////////////////////////////////////////////////////////////////////////

        //然后使用这个宏就可以注册了,分了三个注册的,##表示当成字符串连起来,可以自己看一下宏展开,单个表示字符串,前一部分快速生成了一个createobj的函数(就是生成类的方法)
        //然后使用注册类的构造函数把这个方法和名字直接注入工厂的map里面
        #define REGISTER_CLASS(className)                                       \
            Object * createObject##className()                                  \
            {                                                                   \
                Object * obj = new className();                                 \
                obj->set_class_name(#className);                                \
                return obj;                                                     \
            }                                                                  	\
            ClassRegister classRegister##className(#className, createObject##className)

        //这边使用了黑科技,通过类字段的地址的强转int,减去类的首地址就可以拿到类字段的偏移量,ps:这边也都是创建当前作用域(取决于当前宏位置)的一个register类来实现的
        #define REGISTER_CLASS_FIELD(className, fieldName, fieldType) \
            className className##fieldName; \
            ClassRegister classRegister##className##fieldName(#className, #fieldName, #fieldType, (size_t)(&(className##fieldName.fieldName)) - (size_t)(&className##fieldName))
        //这个地方,第一参是类名,第二参是函数名,第三参是返回值,后面全是参数(按顺序),这个宏的设计还挺牛的,这个宏是支持可变参数的
        #define REGISTER_CLASS_METHOD(className, methodName, returnType, ...) \
            std::function<returnType(className *, ##__VA_ARGS__)> className##methodName##method = &className::methodName; \
            ClassRegister classRegister##className##methodName(#className, #methodName, (uintptr_t)&(className##methodName##method))
    }       //这个##__VA_ARGS__就是可变参数列表展开,然后给这个function赋值为成员函数的地址(记得加&号),然后创建对象(调用构造函数)的时候把指针强转成统一格式丢入即可
}