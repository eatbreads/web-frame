#pragma once

#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <sys/epoll.h>

namespace yazi
{
    namespace socket
    {
        class EventPoller
        {
        public:
            EventPoller();
            ~EventPoller();

            bool create(int max_conn);
            void add(int fd, uint32_t events);
            void mod(int fd, uint32_t events);
            void del(int fd);
            bool is_set(int idx, uint32_t events);
            int wait(int milliseconds);
            int get_fd(int idx);

        private:
            int m_epfd;//epoll 专用的文件描述符
            int m_max_conn;// epoll 最大连接数
            struct epoll_event * m_events;//epoll 用于回传待处理事件的数组
        };
    }
}