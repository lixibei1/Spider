#include "Spider.h"

string ToLower(string str)
{
	int i = 0;
	while (str[i])
	{
		if (str[i] >= 'A'&&str[i] <= 'Z')
		{
			str[i] = str[i] - 'A' + 'a';
		}
		i++;
	}
	return str;
}

CHttp::CHttp()
{
}
CHttp::~CHttp()
{
	WSACleanup();
}
//初始化网络
bool CHttp::Init()
{
	WSADATA wd;
	if (0 != WSAStartup(MAKEWORD(2, 2), &wd))//windows套接字
		return false;
	if (LOBYTE(wd.wVersion) != 2 || HIBYTE(wd.wVersion) != 2)//低位字节，高位字节
		return false;
	//创建套接字
	m_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_socket == INVALID_SOCKET)
		return false;
	return true;
}

//解析网页
bool CHttp::AnalyseUrl(string url)
{
	//字符串大小写转换，全部转为小写
	ToLower(url);
	
	if (string::npos == url.find("http://"))//没找到http://
		return false;
	if (url.length() <= 7)
		return false;
	int pos = url.find('/', 7);//从第7位开始找"/"
	if (pos == string::npos)
	{
		m_host = url.substr(7);//字符串截取，从第7位到最后
		m_object = "/";
	}
	else
	{
		m_host = url.substr(7, pos-7);//第二个参数表示要截取多少个字符
		m_object = url.substr(pos);
	}
	if (m_host.empty())
		return false;
	//cout << "主机名：" << m_host << "\t资源路径" << m_object << endl;
	return true;
}
//连接服务器
bool CHttp::Connect()
{
	hostent *p = gethostbyname(m_host.c_str());
	if (p == NULL)
		return false;
	sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(80);//web服务器默认端口：80 host to net short 主机字节序转换为网络字节序
	memcpy(&sa.sin_addr, p->h_addr, 4);
	
	//连接服务器
	if (SOCKET_ERROR == connect(m_socket, (sockaddr*)&sa, sizeof(sa)))
		return false;

	return true;
}
//获取网页
bool CHttp::FetchHtml(string &html)
{
	//Http请求
	//GET 请求
	string info;  
	info = info + "GET " + m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + m_host + "\r\n";
	info = info + "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36\r\n";  
	info = info + "Connection: Close\r\n";
	info = info + "\r\n";
	//发送给服务器
	if (SOCKET_ERROR == send(m_socket, info.c_str(), info.length(), 0))
		return false;
	//接收
	char ch = 0;
	int nRecv = 0;
	while ((nRecv=recv(m_socket, &ch, sizeof(ch), 0))>0)
	{
		html = html + ch;
	}
	return true;
}
//解析网页
//下载资源
bool CHttp::Download(string url, string filename)
{
	if (false == Init())//初始化网络
		return false;
	if (false == AnalyseUrl(url))//解析url
		return false;
	if (false == Connect())//连接服务器
		return false;
	string info;
	info = info + "GET " + m_object + " HTTP/1.1\r\n";
	info = info + "Host: " + m_host + "\r\n";
	info = info + "User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/74.0.3729.169 Safari/537.36\r\n";
	info = info + "Connection: Close\r\n";
	info = info + "\r\n";

	if (SOCKET_ERROR == send(m_socket, info.c_str(), info.length(), 0))
		return false;
	//打开文件
	FILE* fp = fopen(filename.c_str(), "wb");
	if (fp == NULL)
		return false;
	//接收头信息
	char ch = 0;
	while (recv(m_socket, &ch, 1, 0))
	{
		if (ch == '\r')
		{
			recv(m_socket, &ch, 1, 0);
			if (ch == '\n')
			{
				recv(m_socket, &ch, 1, 0);
				if (ch == '\r')
				{
					recv(m_socket, &ch, 1, 0);
					if (ch == '\n')
						break;
				}
			}
		}
	}

	//接收数据
	while (recv(m_socket, &ch, 1, 0))
	{
		fwrite(&ch, 1, 1, fp);
	}
	//关闭文件
	fclose(fp);
	return true;
}
//编码转换
bool CHttp::UTF8ToANSI(std::string &str1, std::string &str2)
{
	//UTF-8转Unicode
	wchar_t *pUnicodeBuffer = new wchar_t[str1.length()];
	memset(pUnicodeBuffer, 0, sizeof(wchar_t)* str1.length());
	MultiByteToWideChar(CP_UTF8, 0, str1.c_str(), str1.length(), pUnicodeBuffer, sizeof(wchar_t)*str1.length());
	//宽字符转多字节
	char *pBuffer = new char[wcslen(pUnicodeBuffer)*2];
	memset(pBuffer, 0, sizeof(char)*wcslen(pUnicodeBuffer) * 2);

	WideCharToMultiByte(CP_ACP, 0, pUnicodeBuffer, wcslen(pUnicodeBuffer)*2, pBuffer, wcslen(pUnicodeBuffer)*2, 0, 0);
	str2 = pBuffer;
	delete[] pBuffer;
	delete[] pUnicodeBuffer;
	return true;
}
bool CHttp::Close()
{
	closesocket(m_socket);
	m_host.empty();
	m_object.empty();
	m_socket = 0;
	return true;
}