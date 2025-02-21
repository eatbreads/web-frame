#include <web/file_upload.h>
using namespace yazi::web;

#include <utility/logger.h>
using namespace yazi::utility;

#include <fs/file.h>
using namespace yazi::fs;
 
FileUpload::FileUpload() : m_buf(nullptr), m_len(0), m_start(0), m_end(0)
{
}

FileUpload::~FileUpload()
{
}

string FileUpload::name() const
{
    return m_name;
}

string FileUpload::filename() const
{
    return m_filename;
}

string FileUpload::type() const
{
    return m_type;
}

const char * FileUpload::data() const
{
    return m_buf;
}

int FileUpload::size() const
{
    return m_end - m_start;
}

string FileUpload::extension() const
{
    return m_extension;
}

void FileUpload::parse(const char * buf, int len)//感觉这个的调用就是已经解析过报文头了,这是用来特殊解析报文体的
{
    m_buf = buf;
    m_len = len;

    const char * s = buf;   //s表示start
    const char * e = buf + len - 1; //e表示end
    const char * i = s;         //i表示index

    // parse multipart/form-data boundary
    while ((i < e) && (strchr("\r\n", *i) == nullptr)) i++;
    m_boundary = string(s, 2, i - s);   //解析boundary,要从2开始解析,因为正常的boundary是有四个横杠,但是消息体的有六个横杠

    while ((i < e) && (strchr("\r\n", *i) != nullptr)) i++; //这边就是跳过换行
    s = i;

    /* parse form data headers */
    while (i < e)//然后就是解析boundary内部的信息对
    {
        // parse http form data header's name
        while ((i < e) && (strchr(": ", *i) == nullptr)) i++;
        string name = string(s, 0, i - s);  //在示例里面这个name是content_disposition

        while ((i < e) && (strchr(": ", *i) != nullptr)) i++;
        s = i;
 
        // parse http form data header's value
        while ((i < e) && (strchr("\r\n", *i) == nullptr)) i++;
        string value = string(s, 0, i - s); //这个disposition内部先不用解析,直接先存进去
        m_headers[name] = value;            //m_headers是用于存储这些信息对的哈希表

        if (name == "Content-Disposition")//如果是disposition,就在这边解析,解析name和filename
        {
            size_t pos = value.find("name=");
            size_t start = pos + 6;
            size_t end = value.find("\"", start);
            m_name = value.substr(start, end - start);
            pos = value.find("filename=");      //其实不是很懂这里解析的规则的
            start = pos + 10;
            end = value.find("\"", start);
            m_filename = value.substr(start, end - start);
            size_t ext = m_filename.find(".");
            m_extension = m_filename.substr(ext);
        }
        else if (name == "Content-Type")
        {
            m_type = value;
        }

        if (strncmp(i, "\r\n\r\n", 4) == 0)
        {
            i += 4;     //这里要跳过,然后把index加4
            break;
        }

        while ((i < e) && (strchr("\r\n", *i) != nullptr)) i++;//如果只有两个,不是四个的话,就跳过这俩,然后继续循环
        s = i;
    }
    m_start = i - buf;  //表示二进制文件开始的偏移量,注意是偏移量不是地址,指针相减是得到int
    string str = "--" + m_boundary + "--";//这个就是指的是最后一行
    const char * t = e - str.size();//除了最后一行的边界的尾地址
    m_end = t - m_buf;//这个就是结束位置的偏移量
}

bool  FileUpload::save(const string & filename)
{
    File file(filename);//自己封装的文件操作
    if (!file.create())
    {
        log_error("create file error: %s", filename.c_str());
        return false;
    }

    std::ofstream ofs(filename);
    if (ofs.fail())
    {
        log_error("open file failed: %s", filename.c_str());
        return false;
    }
    ofs.write(m_buf + m_start, m_end - m_start);    //从start位置开始写,大小就是结束偏移量减去开始偏移量
    ofs.flush();
    return true;
}
