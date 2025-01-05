#include "XHttpResponse.h"
#include <string>
#include <regex>
bool XHttpResponse::SetRequest(std::string request)
{
	std::string src = request;
	std::string pattern = "^([A-Z]+) /([a-zA-Z0-9]*([.][a-zA-Z]*)?)[?]?(.*) HTTP/1"; // ^开始位置 +后面一个或者多个 （）取出括号中的内容 .任意匹配 *0或者多个
	std::regex r(pattern);
	std::smatch mas; //提取的结果集
	std::regex_search(src, mas, r);
	if (mas.size() == 0)
	{
		printf("%s match failed\n", pattern.c_str());
		return false;
	}
	//mas[0] 是整个匹配一直到/1
	std::string type = mas[1];
	std::string path = "/";
	path += mas[2];
	std::string filetype = mas[3];
	std::string query = mas[4];


	if (type != "GET") {
		printf("Not Get\n");
		return false;
	}
	std::string filename = path;
	if (path == "/")
	{
		filename = "/index.html";
	}

	std::string filepath = "www";
	filepath += filename;

	if(filetype.size() > 0)
		filetype = filetype.substr(1, filetype.size()-1);
	printf("type:[%s]\npath:[%s]\nfiletype:[%s]\nquery:[%s]",type.c_str(),path.c_str(), filetype.c_str(), query.c_str()); 
	if (filetype == "php") {
		std::string cmd = "php-cgi ";
		cmd += filepath;
		cmd += " ";
		for (int i = 0; i < query.size(); ++i) {

			if (query[i] == '&') query[i] = ' ';
		}
		cmd += query;	

		cmd += " > ";
		filepath += ".html";
		cmd += filepath;

		printf("%s", cmd.c_str());
		system(cmd.c_str());
	}
	
	fp = fopen(filepath.c_str(), "rb");
	if (fp == NULL) //需要先手动执行php-cgi命令 生成index.php.html文件 因为windows下不支持UNC路径 需要添加到注册表
	{
		printf("open file %s failed\n", filepath.c_str());
		return false;
	}
	//获取文件大小
	fseek(fp, 0, SEEK_END); //把文件指针移到结尾处 从结尾处移0个
	filesize = ftell(fp); //缺陷 只能保存4G左右 符号还有1~2G
	fseek(fp, 0, 0); //将指针移回头部
	printf("\nfile size is %d\n", filesize);

	if (filetype == "php") {
		char c = 0;
		//\r\n\r\n
		int headsize = 0;
		while (fread(&c, 1, 1, fp) > 0)
		{
			++headsize;
			if (c == '\r')
			{
				fseek(fp, 3, SEEK_CUR); //从第一\r的当前位置移动三位 移到结尾处
				headsize += 3;
				break;
			}
		}
		filesize = filesize - headsize;
	}

	return true;
}

std::string XHttpResponse::GetHead()
{
	//消息头
	std::string rmsg = "";
	rmsg = "HTTP/1.1 200 OK\r\n";
	rmsg += "Server: XHttp\r\n";
	rmsg += "Content-Type: text/html\r\n";
	rmsg += "Content-Length: ";
	char bsize[128] = { 0 };
	sprintf(bsize, "%d", filesize);
	rmsg += bsize;
	//rmsg += "10\r\n";
	rmsg += "\r\n\r\n"; //结尾标识 协议约定好的

    return rmsg;
}

int XHttpResponse::Read(char* buffer, int bufferSize)
{

	//发送正文
	return fread(buffer, 1,bufferSize, fp);
}

XHttpResponse::XHttpResponse()
{
     
}

XHttpResponse::~XHttpResponse()
{
}
