#include <frame/server.h>
using namespace yazi::frame;

#include <thread/task_dispatcher.h>
using namespace yazi::thread;

#include <socket/socket_handler.h>
using namespace yazi::socket;

void Server::start()
{   //这样子使用Singleton就可以实现单例,
    auto sys = Singleton<System>::instance();       //创建系统类
    sys->init();                                    //给系统初始化,执行吐核权限和打开日志操作
    string root_path = sys->get_root_path();

    // ini config
    auto ini = Singleton<IniFile>::instance();      //载入配置文件并且初始化
    ini->load(root_path + "/config/server.ini");

    m_ip = (string)(*ini)["server"]["ip"];          //从ini类里面拿到配置文件的键值对
    m_port = (*ini)["server"]["port"];
    m_threads = (*ini)["server"]["threads"];        //里面重载了operator
    m_max_conns = (*ini)["server"]["max_conns"];
    m_wait_time = (*ini)["server"]["wait_time"];
    m_log_level = (*ini)["server"]["log_level"];

    // ini logger
    auto logger = Singleton<Logger>::instance();
    logger->open(root_path + "/log/server.log"); 
    logger->set_level(m_log_level);                 //打开日志,设置等级
    logger->set_console(false);

    // init the thread pool and task queue
    auto dispatcher = Singleton<TaskDispatcher>::instance();    //初始化分配器,内部有任务队列,任务队列内部有线程池
    dispatcher->init(m_threads);

    // init the socket handler
    auto handler = Singleton<SocketHandler>::instance();
    handler->listen(m_ip, m_port);                              //开始监听ip和端口
    handler->handle(m_max_conns, m_wait_time);
}
