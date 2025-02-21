#pragma once

#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
using std::string;

#include <utility/logger.h>
using namespace yazi::utility;

namespace yazi
{
    namespace socket
    {
        class Socket
        {
        public:
            Socket();
            Socket(int sockfd);
            virtual ~Socket();

            int fd() const;
            bool bind(const string & ip, int port);
            bool listen(int backlog);
            bool connect(const string & ip, int port);
            int accept();
            int send(const char * buf, int len);
            int recv(char * buf, int len);
            void close();

            bool set_non_blocking();                  //在非阻塞模式下，如果操作不能立即完成（比如，没有数据可读或缓冲区已满无法写入），操作会立即返回一个错误
            bool set_send_buffer(int size);             //设置socket的发送缓冲区大小
            bool set_recv_buffer(int size);             //设置socket的接收缓冲区大小
            bool set_linger(bool active, int seconds);//这个函数控制socket在关闭时的行为。如果启用了linger选项（active为true），则当socket关闭时，如果仍有数据未发送完毕，socket将在关闭之前等待指定的秒数（seconds）
            bool set_keepalive();//这个函数启用socket的keepalive机制。keepalive机制允许TCP连接在空闲一段时间后发送探测包，以检查对方是否仍然可达。
            bool set_reuseaddr();       //这个函数允许socket地址被重用

        protected:
            string m_ip;
            int m_port;
            int m_sockfd;
        };
    }
}