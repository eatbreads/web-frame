#include <task/task_factory.h>
#include <task/http_task.h>
#include <auto_lock.h>
using namespace yazi::task;

Task * TaskFactory::create(int sockfd)      //这里要判断里面是不是已经有过了
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto it = m_sock_task.find(sockfd);
    if (it != m_sock_task.end())
    {
        return it->second;
    }
    auto task = new HttpTask(sockfd);
    m_sock_task[sockfd] = task;
    return task;
}

void TaskFactory::remove(int sockfd)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto it = m_sock_task.find(sockfd);
    if (it != m_sock_task.end())
    {
        delete it->second;
        m_sock_task.erase(it);
    }
    ::close(sockfd);
}