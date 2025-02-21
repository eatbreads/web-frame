#include <web/server.h>
using namespace yazi::web;

#include <utility/string.h>
using namespace yazi::utility;

#include <reflect/class_factory.h>
using namespace yazi::reflect;

void Server::start()
{
    try
    {
        Singleton<yazi::frame::Server>::instance()->start();
    }
    catch (std::exception & e)
    {
        std::cout << e.what() << std::endl;
    }
} 

void Server::bind(const string & path, server_handler handler)
{
    m_handlers[path] = handler;
}

string Server::handle(const Request & req)
{
    const string & path = req.path();           //先拿到url里面的路径
    auto it = m_handlers.find(path);            //在map里卖弄找到这个路径对应的方法
    if (it != m_handlers.end())
    {
        Response resp;
        it->second(req, resp);                  //找到了就直接处理,然后返回这个数据即可
        return resp.data();
    }

    if (String::has_suffix(path, ".html"))      //表示已这个.html结尾
    {
        const string & filename = get_template_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;                 //这里感觉可以直接调用response的render,up的一开始代码也是这么写的
            oss << ifs.rdbuf();
            resp.data(Response::HTML, oss.str());   //把这个里面的东西全部返回即可
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".js"))
    {
        const string & filename = get_static_folder() + path;   //注意看这里,js和css是static,而不是template
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::JS, oss.str());
            return resp.data();
        }
    }
    
    if (String::has_suffix(path, ".css"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::CSS, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".jpg"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::JPG, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".png"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::PNG, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".gif"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::GIF, oss.str());
            return resp.data();
        }
    }

    if (String::has_suffix(path, ".ico"))
    {
        const string & filename = get_static_folder() + path;
        std::ifstream ifs(filename);
        if (ifs.good())
        {
            Response resp;
            std::ostringstream oss;
            oss << ifs.rdbuf();
            resp.data(Response::ICO, oss.str());
            return resp.data();
        }
    }

    string classname;
    string methodname;
    std::vector<string> arr = String::split(String::trim(path, " /"), '/');     //按理来说,这里会被分成两个,前者是控制器名称,后者是对应要的路径
    if (arr.size() == 1)
    {
        if (arr[0].empty())
        {
            classname = "Index";                   //两个都没有就都是index
            methodname = "index";
        }
        else
        {
            classname = String::capitalize(arr[0]);             //这里转换成大写用来反射的获取
            methodname = "index";                               //如果后者没有的话就是默认index了
        }
    }
    else if (arr.size() == 2)
    {
        classname = String::capitalize(arr[0]);
        methodname = arr[1];
    }
    ClassFactory * factory = Singleton<ClassFactory>::instance();       //这些类在已经被注册过到工厂里面了
    Object * ctrl = factory->create_class(classname);       //从工厂中获取这个类
    if (ctrl == nullptr)
    {
        return Response::page_not_found();
    }
    auto method = Singleton<ClassFactory>::instance()->get_class_method(classname, methodname);
    if (method == nullptr)
    {
        delete ctrl;
        return Response::page_not_found();
    }
    try
    {
        Response resp;
        ctrl->call(methodname, req, resp);          //就是针对具体的操作执行对应的请求
        delete ctrl;
        return resp.data();                         //这里可能会拿到File类,当别人上传文件的时候,然后就会去执行File的upload操作
    }
    catch (std::exception & e)//捕获异常返回404
    {
        Response resp;
        resp.code(404);
        std::ostringstream oss;
        oss << "<h1 style=\"text-align: center;\">404 Page Not Found</h1><p style=\"text-align: center;\">" << e.what() << "</p>";
        resp.data(Response::HTML, oss.str());
        delete ctrl;
        return resp.data();
    }
}

void Server::set_template_folder(const string & template_folder)
{
    m_template_folder = template_folder;
}

string Server::get_template_folder() const
{
    const string & root_path = Singleton<System>::instance()->get_root_path();
    return root_path + "/" + m_template_folder;
}

void Server::set_static_folder(const string & static_folder)
{
    m_static_folder = static_folder;
}

string Server::get_static_folder() const
{
    const string & root_path = Singleton<System>::instance()->get_root_path();
    return root_path + "/" + m_static_folder;
}
