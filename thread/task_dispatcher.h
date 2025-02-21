#pragma once

#include <list>
#include <thread/thread.h>
#include <thread/thread_pool.h>
#include <thread/task.h>

namespace yazi
{
    namespace thread
    {
        class TaskDispatcher : public Thread 
        {
            SINGLETON(TaskDispatcher);
        public:
            void init(int threads);             //初始化线程,线程数量为threads,内部是个线程池
            void assign(Task * task);           //这就是个"任务类",有分发任务,处理任务
            void handle(Task * task);
            bool empty(); 
            virtual void run();

        protected:
            std::list<Task *>   m_queue;        //任务队列
            Mutex               m_mutex;
            Condition           m_cond;
        }; 
    }
}
