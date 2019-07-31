#include "Spider.h"
int main()
{
	//开始界面
	cout << "\t\t\t\t************************************************" << endl;
	cout << "\t\t\t\t-           欢迎使用智能网络爬虫系统           -" << endl;
	cout << "\t\t\t\t************************************************" << endl;
	cout << "\t\t\t\t请输入要抓取的URL链接：" << endl;

	string url;
	cin >> url;
	//创建文件夹  Windows API
	//相对路径
	CreateDirectory(L"./image", NULL);

	//开始抓取
	StartCatch(url);

	return 0;
}
bool StartCatch(string url)
{
	queue<string> q;
	q.push(url);
	while (!q.empty())
	{
		//抓取
		//取出URL
		string currentUrl = q.front();
		q.pop();

		CHttp http;
		//1.初始化网络
		http.Init();
		//2.解析URL
		http.AnalyseUrl(currentUrl);
		//3.连接服务器
		if (false == http.Connect())
			cout << "连接失败" << endl;
		else
			cout << "连接成功" << endl;
		//4.获取网页
		string html;
		string newhtml;
		http.FetchHtml(html);
		http.UTF8ToANSI(html, newhtml);
		//cout << newhtml << endl;

		//5解析网页
		vector<string> vecImage;
		smatch mat;
		regex rex("http://[^\\s'\"<>()]+");
		string::const_iterator start = html.begin();
		string::const_iterator end = html.end();
		while (regex_search(start, end, mat, rex))
		{
			string per(mat[0].first, mat[0].second);
			//cout << per << endl;
			//匹配图片链接
			if (per.find(".jpg") != string::npos || per.find(".png") != string::npos || per.find(".jpeg") != string::npos)
			{
				vecImage.push_back(per);
				cout << per << endl;
			}
			else
			{
				if (per.find("http://www.w3.org/") == string::npos)
					q.push(per);
			}
			start = mat[0].second;
		}
		//6.下载资源
		for (int i = 0; i < vecImage.size(); i++)
		{
			string filename = "F:/VS2013/Visual Studio 2013/Projects/Spider/Spider/image/" +
				vecImage[i].substr(vecImage[i].find_last_of('/') + 1);
			CHttp httpDownload;
			if (false == httpDownload.Download(vecImage[i], filename))
				cout << "下载失败" << GetLastError() << endl;
			else
				cout << "下载成功" << endl;
		}

	}
	return true;
}