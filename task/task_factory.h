#pragma once

#include <map>
#include <mutex>

#include <utility/singleton.h>
using namespace yazi::utility;

#include <socket/socket.h>
using namespace yazi::socket;

#include <thread/task.h>
using namespace yazi::thread;

namespace yazi
{
    namespace task
    {
        class TaskFactory
        {
            SINGLETON(TaskFactory);
        public:
            Task * create(int sockfd);
            void remove(int sockfd);            //添加一个remove方法

        private:
            std::map<int, Task *> m_sock_task;          //这个是用来处理长消息的,如果一个发送很长的话,往往是分几次发送的,此时数据的拼接就会有问题,所以我们希望每一个连接都可以使用同一个task    
            std::mutex m_mutex;                         
        };
    } 
}
