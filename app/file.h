#pragma once

#include <web/controller.h>
using namespace yazi::web;

namespace yazi
{
    namespace app
    {
        class File : public Controller 
        {
        public:
            void index(const Request & req, Response & resp);//用于和之前一样渲染一个html
            void upload(const Request & req, Response & resp);//把对应的内容载入目录
        };
    }
}
