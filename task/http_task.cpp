#include <task/http_task.h>
#include <task/task_factory.h>
using namespace yazi::task;

#include <socket/socket_handler.h>
using namespace yazi::socket;

#include <web/server.h>
using namespace yazi::web;


HttpTask::HttpTask(int sockfd) : Task(), m_sockfd(sockfd)
{
    m_total_len = 0;
    m_head_len = 0;
    m_body_len = 0;
    m_body_idx = 0;
    m_body = nullptr;
}

HttpTask::~HttpTask()
{
}

void HttpTask::reset()
{
    m_total_len = 0;
    m_head_len = 0;
    m_body_len = 0;
    m_body_idx = 0;
    if (m_body != nullptr) 
    {
        delete [] m_body;
        m_body = nullptr;
    }
}

void HttpTask::run()
{
    log_debug("http task run: conn=%d", m_sockfd);

    // 接收客户端的数据
    char buf[recv_buff_size] = {0};         //这个buf是在栈区的缓冲区,body是用来接收消息体的
    Socket socket(m_sockfd);                //如果是第一次发送过来,可能buf里面又有消息头又有消息体,后面就全是消息体了
    int len = socket.recv(buf, sizeof(buf));
    if (len < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            log_debug("socket recv would block: conn=%d", m_sockfd);
            return;
        }
        else if (errno == EINTR)
        {
            log_debug("socket recv interrupted: conn=%d", m_sockfd); 
            return;
        }
        log_debug("socket connection abort: conn=%d", m_sockfd);
        m_closed = true;
        return;
    }
    else if (len == 0)
    {
        log_debug("socket closed by peer: conn=%d", m_sockfd);
        m_closed = true;
        return;
    }

    log_debug("recv: conn=%d msg=\n%s", m_sockfd, buf);

    m_total_len += len;         //这里应该就是+=,因为他可能之前处理过了,被复用了,为了处理大请求
    if (m_head_len == 0)        //如果这个head_len一开始是0的话,说明是第一次收到这个请求
    {
        m_head_len = m_req.parse_header(buf, len);//解析报文头,报文头的数据会在m_req里面
        m_body_len = m_req.content_length();
        // log_info("recv head size=%d, header-length=%d, content-length=%d", len, m_head_len, m_body_len);
        if (m_body_len > 0) 
        {
            m_body = new char[m_body_len + 1]; 
            m_body[m_body_len] = 0;
            std::memcpy(m_body, buf + m_head_len, len - m_head_len);    //如果有就存储到m_body里面
            m_body_idx = len - m_head_len;                      //这边是定位当前m_body的尾后长度,方便后续追加
        }
    }
    else    //后续就是整个直接拷贝进去,然后更新body_idx
    {
        std::memcpy(m_body + m_body_idx, buf, len);
        m_body_idx += len;
    }
    if (m_total_len - m_head_len >= m_body_len)//当前长度减去头部长度大于消息体长度(header里面拿到的),就代表结束了
    {
        // log_info("recv body complete: body-len=%d, body-data=\n%s", m_body_idx, m_body);
        m_req.parse_body(m_body, m_body_len);//处理完了就可以开始解析body了
        auto server = Singleton<Server>::instance();
        string output = server->handle(m_req);//然后调用server处理请求
        socket.send(output.c_str(), output.size());//然后发送回去
        reset();        //在这里释放资源,而不是在析构函数
    }
}

void HttpTask::destroy()
{
    log_debug("http task destroy: conn=%d", m_sockfd);
    if (m_closed)
    {
        Singleton<TaskFactory>::instance()->remove(m_sockfd);
    }
    else
    {
        Singleton<SocketHandler>::instance()->attach(m_sockfd);
    }
}
