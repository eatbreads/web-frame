#include <app/file.h>
using namespace yazi::app;

CONTROLLER(File);

ACTION(File, index);
void File::index(const Request & req, Response & resp)
{
    resp.render("file/index.html");
}

ACTION(File, upload);
void File::upload(const Request & req, Response & resp)
{
    FileUpload file = req.file("file");//FileUpload就是之前的解析文件上传的类,里面有一个save操作
 
    const string & filename = "./upload/" + file.filename();
    file.save(filename);//把东西上传进去了
    
    Json json;
    json["code"] = 0;
    json["data"] = "上传成功";
    return resp.json(json.str());   //然后返回一个json给服务器
}