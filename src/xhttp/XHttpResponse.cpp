#include "XHttpResponse.h"
#include <string>
#include <regex>
bool XHttpResponse::SetRequest(std::string request)
{
	std::string src = request;
	std::string pattern = "^([A-Z]+) /([a-zA-Z0-9]*([.][a-zA-Z]*)?)[?]?(.*) HTTP/1"; // ^��ʼλ�� +����һ�����߶�� ����ȡ�������е����� .����ƥ�� *0���߶��
	std::regex r(pattern);
	std::smatch mas; //��ȡ�Ľ����
	std::regex_search(src, mas, r);
	if (mas.size() == 0)
	{
		printf("%s match failed\n", pattern.c_str());
		return false;
	}
	//mas[0] ������ƥ��һֱ��/1
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
	if (fp == NULL) //��Ҫ���ֶ�ִ��php-cgi���� ����index.php.html�ļ� ��Ϊwindows�²�֧��UNC·�� ��Ҫ��ӵ�ע���
	{
		printf("open file %s failed\n", filepath.c_str());
		return false;
	}
	//��ȡ�ļ���С
	fseek(fp, 0, SEEK_END); //���ļ�ָ���Ƶ���β�� �ӽ�β����0��
	filesize = ftell(fp); //ȱ�� ֻ�ܱ���4G���� ���Ż���1~2G
	fseek(fp, 0, 0); //��ָ���ƻ�ͷ��
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
				fseek(fp, 3, SEEK_CUR); //�ӵ�һ\r�ĵ�ǰλ���ƶ���λ �Ƶ���β��
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
	//��Ϣͷ
	std::string rmsg = "";
	rmsg = "HTTP/1.1 200 OK\r\n";
	rmsg += "Server: XHttp\r\n";
	rmsg += "Content-Type: text/html\r\n";
	rmsg += "Content-Length: ";
	char bsize[128] = { 0 };
	sprintf(bsize, "%d", filesize);
	rmsg += bsize;
	//rmsg += "10\r\n";
	rmsg += "\r\n\r\n"; //��β��ʶ Э��Լ���õ�

    return rmsg;
}

int XHttpResponse::Read(char* buffer, int bufferSize)
{

	//��������
	return fread(buffer, 1,bufferSize, fp);
}

XHttpResponse::XHttpResponse()
{
     
}

XHttpResponse::~XHttpResponse()
{
}
