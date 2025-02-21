#include <web/request.h>
using namespace yazi::web;

#include <string.h>
#include <vector>

#include <utility/string.h>
#include <utility/logger.h>
using namespace yazi::utility;

int Request::parse_header(const char * buf, int len) 
{
    /* parse request line: method, URI, proto */
    const char * s = buf;
    const char * e = buf + len - 1;//拿到尾部
    const char * i = s;             //用于遍历的指针

    while ((i < e) && (isspace(*i))) i++;
    s = i;                          //s用于指向第一个不是空

    // parse http request's method
    while ((i < e) && (strchr(" ", *i) == NULL)) i++;       //即这个strchr可以简单理解为在第一参内部找第二参的char,返回对应指针
    m_method = string(s, 0, i - s); //解析get/put等

    while ((i < e) && (strchr(" ", *i) != NULL)) i++;       //用于跳过空字符,如果有的话
    s = i;
 
    // parse http request's uri
    while ((i < e) && (strchr(" ", *i) == NULL)) i++;       //获取到uri(可简单理解为带问号的路径)
    m_uri = string(s, 0, i - s);

    while ((i < e) && (strchr(" ", *i) != NULL)) i++;       //用于跳过空字符,如果有的话
    s = i;

    // parse http request's protocol
    while ((i < e) && (strchr("\r\n", *i) == NULL)) i++;    //i会停在\r\n的前一个,故可以拿到第一行的结尾http版本
    m_proto = string(s, 0, i - s);

    while ((i < e) && (strchr("\r\n", *i) != NULL)) i++;    //跳过\r\n,这样交替来写特别有用
    s = i;

    /* if URI contains '?' character, initialize query_string */
    size_t pos = m_uri.find_first_of('?');
    if (pos != string::npos)
    {
        m_path = m_uri.substr(0, pos);
        m_query_string = m_uri.substr(pos + 1);     //分别获取路径和请求语句

        std::vector<string> output = String::split(m_query_string, '&');    //用于分割请求语句ps这个String是自己写的
        for (auto it = output.begin(); it != output.end(); ++it)
        {
            std::vector<string> out = String::split((*it), '=');//对切割出来的再从等于号切割
            if (out.size() == 2)
            {
                m_get[out[0]] = out[1];
            }
            else
            {
                m_get[out[0]] = "";
            }
        }
    }
    else//如果没有问号就是直接就是uri了
    {
        m_path = m_uri;
    }

    while (i < e)               //然后就是解析请求头
    {
        // parse http request header's name
        while ((i < e) && (strchr(": ", *i) == NULL)) i++;//这边是直到定位到冒号或者空格
        string name = string(s, 0, i - s);

        while ((i < e) && (strchr(": ", *i) != NULL)) i++; //直到定位到非冒号或者空格,就重新确定s
        s = i;

        // parse http request header's value
        while ((i < e) && (strchr("\r\n", *i) == NULL)) i++;
        string value = string(s, 0, i - s);
        m_headers[name] = value;

        if (strncmp(i, "\r\n\r\n", 4) == 0)     //如果遇到连续的四个这玩意,就意味着循环应该结束了,已经解析结束
        {
            i += 4;
            break;
        }

        while ((i < e) && (strchr("\r\n", *i) != NULL)) i++;//如果没有结束就跳过两个继续判断
        s = i;                       
    }
    return i - buf;
}

void Request::parse_body(const char * buf, int len)//post的内容是放在body里面的,所以要单独来解析这个body
{
    if (len <= 0)
    {
        return;
    }
    m_body = buf;
    const string & content_type = header("Content-Type");
    if (content_type.find("application/json") != string::npos)
    {
        log_info("body data=\n%s", m_body.c_str());
        m_post.load(m_body.c_str(), m_body.size());
        return;
    }
    else if (content_type.find("application/x-www-form-urlencoded") != string::npos)
    {
        log_info("body data=\n%s", m_body.c_str());
        std::vector<string> output = String::split(m_body, '&');
        for (auto it = output.begin(); it != output.end(); ++it)
        {
            std::vector<string> out = String::split((*it), '=');
            if (out.size() == 2)
            {
                m_post[out[0]] = out[1];
            }
            else
            {
                m_post[out[0]] = "";
            }
        }
    }
    else if (content_type.find("multipart/form-data") != string::npos)  //如果这边是文件上传,就是调用fileupload这个类来解析
    {
        FileUpload upload;
        upload.parse(buf, len);
        m_files[upload.name()] = upload;
        return;
    }
}

bool Request::is_get() const
{
    return (m_method == "GET");
}

bool Request::is_post() const
{
    return (m_method == "POST");
}

string Request::get(const string & name) const
{
    auto it = m_get.find(name);
    if (it != m_get.end())
    {
        return it->second;
    }
    return "";
}

Json Request::post(const string & name) const
{
    if (name == "")
    {
        return m_post;
    }
    return m_post.get(name);
}

string Request::header(const string & name) const
{
    auto it = m_headers.find(name);
    if (it != m_headers.end())
    {
        return it->second;
    }
    return "";
}

string Request::cookie(const string & name) const//这边暂时随便写,不一定会有cookie
{
    auto it = m_cookies.find(name);
    if (it != m_cookies.end())
    {
        return it->second;
    }
    return "";
}

string Request::path() const
{
    return m_path;
}

string Request::user_host() const
{
    return header("Host");
}

string Request::user_agent() const
{
    return header("User-Agent");
}

int Request::content_length()
{
    return std::atoi(header("Content-Length").c_str());
}

FileUpload Request::file(const string & name) const
{
    auto it = m_files.find(name);
    if (it != m_files.end())
    {
        return it->second;
    }
    return FileUpload();//哈希里面没找到就返回空
}

void Request::show() const
{
    log_debug("http method: %s", m_method.c_str());
    log_debug("http uri: %s", m_uri.c_str());
    log_debug("http proto: %s", m_proto.c_str());
    log_debug("http path: %s", m_path.c_str());
    log_debug("http query string: %s", m_query_string.c_str());
    log_debug("http headers -- start");
    for (auto it = m_headers.begin(); it != m_headers.end(); ++it)
    {
        log_debug("http header: %s=%s", it->first.c_str(), it->second.c_str());
    }
    log_debug("http headers -- end");
    log_debug("http get params -- start");
    for (auto it = m_get.begin(); it != m_get.end(); ++it)
    {
        log_debug("http get: %s=%s", it->first.c_str(), it->second.c_str());
    }
    log_debug("http get params -- end");
    log_debug("http post params -- start");
    log_debug("http post %s", m_post.str().c_str());
    log_debug("http post params -- end");
    log_debug("http body: %s", m_body.c_str());
}
