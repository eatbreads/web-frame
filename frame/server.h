#pragma once

#include <string>
using std::string;

#include <utility/system.h>
#include <utility/logger.h>
#include <utility/ini_file.h>
#include <utility/singleton.h>
using namespace yazi::utility;

namespace yazi
{
    namespace frame
    {
        class Server
        {
            SINGLETON(Server);
        public:
            void start();//创建系统类并初始化,载入配置文件并且初始化,打开日志,设置等级
                         //初始化分配器,内部有任务队列,任务队列内部有线程池,并且开始监听
        private:
            string m_ip;
            int m_port = 0;
            int m_threads = 0;
            int m_max_conns = 0;
            int m_wait_time = 0;
            int m_log_level = 0;
        };
    }
}
