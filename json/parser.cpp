#include <json/parser.h>
using namespace yazi::json;

void Parser::load(const string & filename)
{
    std::ifstream ifs(filename);
    std::ostringstream oss;
    oss << ifs.rdbuf();
    m_str = oss.str();
    m_idx = 0;
}

void Parser::load(const char * buf, int len)
{
    m_str.assign(buf, len);
    m_idx = 0;
}

void Parser::skip_white_space()
{       //这里忽略全部的空白符,包括回车换行
    while (m_str[m_idx] == ' ' || m_str[m_idx] == '\r' || m_str[m_idx] == '\n' || m_str[m_idx] == '\t')
        m_idx++;
}

char Parser::get_next_token() 
{   //从缓冲区里面读取一个,忽略空白符
    skip_white_space();
    if (m_idx == m_str.size())
        throw std::logic_error("unexpected end of input");
    return m_str[m_idx++];
}

Json Parser::parse()
{
    char ch = get_next_token();
    switch (ch)
    {//这里就是先看一眼是什么,是n那就可能是null,然后m_idx--
        case 'n':
            m_idx--;
            return parse_null();//然后对此时的m_idx执行解析
        case 't':
        case 'f':       //可能是true或者false
            m_idx--;
            return parse_bool();
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            m_idx--;
            return parse_number();//这里说明是数字
        case '"':   //双引号说明是字符串
            return Json(parse_string());
        case '[':       //[]说明是数组 
            return parse_array();
        case '{':       //说明是键值对(这样设计还很合理,每一种都是不同的)
            return parse_object();
        default:
            break;
    }
    throw std::logic_error("unexpected character in parse json");
}

Json Parser::parse_null()
{
    if (m_str.compare(m_idx, 4, "null") == 0)
    {
        m_idx += 4;
        return Json();
    }
    throw std::logic_error("parse null error");
}

Json Parser::parse_bool()
{
    if (m_str.compare(m_idx, 4, "true") == 0)
    {
        m_idx += 4;
        return Json(true);
    }
    if (m_str.compare(m_idx, 5, "false") == 0)
    {
        m_idx += 5;
        return Json(false);
    }
    //如果这两行if都没进去就会抛异常
    throw std::logic_error("parse bool error");
}

Json Parser::parse_number()
{
    size_t pos = m_idx;     //pos拿到的是一开始的位置

    if (m_str[m_idx] == '-')
        m_idx++;

    // integer part
    if (m_str[m_idx] == '0')
    {
        m_idx++;
    }
    else if (in_range(m_str[m_idx], '1', '9'))
    {
        m_idx++;//这里先把整数部分先给读完了
        while (in_range(m_str[m_idx], '0', '9'))
        {
            m_idx++;
        }
    }
    else
    {
        throw std::logic_error("invalid character in number");
    }

    if (m_str[m_idx] != '.')//如果不存在小数的话
    {
        return Json(std::atoi(m_str.c_str() + pos));//如果不是浮点就调用这个
    }

    // decimal part
    m_idx++;
    if (!in_range(m_str[m_idx], '0', '9'))//如果存在就走到这里
    {
        throw std::logic_error("at least one digit required in fractional part");
    }
    while (in_range(m_str[m_idx], '0', '9'))
    {
        m_idx++;
    }   //所以说这上面一大堆只是为了移动m_idx来支持后续的解析
    return Json(std::atof(m_str.c_str() + pos));//其实一开始直接调用这个也可以
}

string Parser::parse_string()
{
    int pos = m_idx;//这里还挺复杂的
    while (true)
    {//while循环内部不断地获取下一个字符
        char ch = get_next_token();//这个get已经是忽略特殊字符和空白符了
        if (ch == '"')
        {
            break;
        }

        // The usual case: non-escaped characters
        if (ch == '\\') //这边应该使用了两个\\来表示一个\,说明遇到一些其他东西
        {//json里面的\和cpp里面的\n这种应该还是不同的cpp这种应该算一个字符,即使是\+n
            ch = m_str[m_idx++];
            switch (ch)
            {
                case 'b':
                case 't':
                case 'n':
                case 'f':
                case 'r':
                case '"':
                case '\\':
                    break;
                case 'u':       //这是utf8这种吧?要直接加
                    m_idx += 4;
                    break;
                default:
                    break;
            }
        }
    }
    return m_str.substr(pos, m_idx - pos - 1);  //然后直接返回中间这一串即可
}

Json Parser::parse_array()//此函数的调用是检测到了"["才进来,所以不用扫描[了
{
    Json arr(Json::JSON_ARRAY);//上面那些好像没有设置json属性
    char ch = get_next_token();
    if (ch == ']')      //这边是为了看一下是否是空数组
    {
        return arr;
    }
    m_idx--;
    while (true)
    {   //这边是append往vector尾部追加(parse会返回一个json)
        arr.append(parse());//使用了递归的手法,因为内部也是各种类型
        ch = get_next_token(); 
        {
            break;              //这里是真正的函数出口
        }
        if (ch != ',')          //如果又不是出口,又不是",",就抛出异常,是逗号就while继续处理
        {
            throw std::logic_error("expected ',' in array");//这些写异常的操作还是很不错的,不然找日志烦得很
        }
    }
    return arr;
}

Json Parser::parse_object()
{
    Json obj(Json::JSON_OBJECT);
    char ch = get_next_token();
    if (ch == '}')
    {
        return obj;
    }
    m_idx--;
    while (true)
    {
        ch = get_next_token();  //先解析key
        if (ch != '"')
        {
            throw std::logic_error("expected '\"' in object");
        }
        string key = parse_string();
        ch = get_next_token();
        if (ch != ':')
        {
            throw std::logic_error("expected ':' in object");
        }
        obj[key] = parse();     //这里应该等价于map,可以有多个key/value
        ch = get_next_token();
        if (ch == '}')
        {
            break;
        }
        if (ch != ',')//如果它的key/value是有多个的,就还可以继续解析
        {
            throw std::logic_error("expected ',' in object");
        }
    }
    return obj;
}