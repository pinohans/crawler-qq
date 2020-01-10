// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <SDKDDKVer.h>
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <windows.h>
#include <iostream>

#include <string>
#include <filesystem>
#include <sstream>

#include "../common/include/curl/curl.h"
#include "../common/convert.h"
#include "../common/httoIO.h"
#include "../common/logger.h"
#include "../common/include/json/json.hpp"
#include "../common/include/csv/reader.hpp"
using json = nlohmann::json;

int update()
{
	logger* log = new logger("log");
	httpIO http(log);
	std::string ret = http.Post("http://192.168.226.1:8000/anonymousApi", "{\"type\":\"0\"}");

	json resp = json::parse(ret.c_str());
	if (resp["error"] == "0")
	{
		json data = resp["data"];
		for (json::iterator it = data.begin(); it != data.end(); it++)
		{
			std::string filename = (*it)["filename"].get<std::string>();
			std::string url = (*it)["url"].get<std::string>();
			std::string md5 = (*it)["md5"].get<std::string>();


			if (md5 != Getmd5(filename))
			{
				log->doLog("[debug]", Getmd5(filename).c_str());
				if (http.Download(url, filename)) {
					continue;
				}
				return 0;
			}
		}
		return 1;
	}
	return 0;
}

int main()
{
	json j = json::parse("");
	system("pause");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
