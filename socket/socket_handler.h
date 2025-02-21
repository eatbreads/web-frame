#pragma once

#include <map>
#include <socket/server_socket.h>
#include <socket/event_poller.h>

namespace yazi
{
    namespace socket
    {
        class SocketHandler
        {
            SINGLETON(SocketHandler);       //执行了这个宏函数,就会自己生成单例了
        public:
            void listen(const string & ip, int port);
            void attach(int sockfd);
            void detach(int sockfd);
            void handle(int max_conn, int timeout);

        private:
            Socket * m_server = nullptr;
            EventPoller m_epoll;
        };
    }
}