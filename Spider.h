#include <iostream>
#include <string>
#include <Windows.h>
#include <queue>
#include <regex>
#include <vector>

using namespace std;
#pragma comment(lib,"ws2_32.lib")

//开始抓取
bool StartCatch(string url);

//http操作
class CHttp
{
private:
	string m_host;                                           //域名
	string m_object;                                         //资源路径
	SOCKET m_socket;                                         //套接字
public:
	CHttp();
	~CHttp();
	bool Init();                                             //初始化网络
	bool AnalyseUrl(string url);                             //解析网页
	bool Connect();                                          //连接服务器
	bool FetchHtml(string &html);                            //获取网页
	                                                         //解析网页
	bool Download(string url, string filename);              //下载资源
	bool UTF8ToANSI(std::string &str1, std::string &str2);   //编码转换
	bool Close();                                            //关闭套接字
};