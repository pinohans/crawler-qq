// test.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <SDKDDKVer.h>
#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展
#include <windows.h>
#include <iostream>

#include <string>
#include <filesystem>
#include <sstream>
#include "../common/sqlite3.h"
int main()
{
	sqlite3 *sql = NULL; // 一个打开的数据库实例
	std::string a = u8"'#@&^!@哈哈哈";

	int result = sqlite3_open_v2("test.db", &sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
	if (result == SQLITE_OK) {
		
		sqlite3_stmt *stmt = NULL;        //stmt语句句柄

		//进行插入前的准备工作——检查语句合法性
		//-1代表系统会自动计算SQL语句的长度
		int result = sqlite3_prepare_v2(sql, "CREATE TABLE IF NOT EXISTS log( id integer PRIMARY KEY AUTOINCREMENT, module text NOT NULL, time text NOT NULL, level text NOT NULL, message text NOT NULL ); ", -1, &stmt, NULL);

		if (result == SQLITE_OK) {
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			result = sqlite3_prepare_v2(sql, "INSERT INTO log(module, time, level, message) VALUES(?,?,?,?);", -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_text(stmt, 1, a.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_text(stmt, 2, a.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_text(stmt, 3, a.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_text(stmt, 4, a.c_str(), -1, SQLITE_STATIC);
				std::cout << "c" << std::endl;
				sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}

		result = sqlite3_prepare_v2(sql, "SELECT module, time, level, message FROM log;", -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				const unsigned char* sModule = sqlite3_column_text(stmt, 0);
				const unsigned char* sTime = sqlite3_column_text(stmt, 1);
				const unsigned char* sLevel = sqlite3_column_text(stmt, 2);
				const unsigned char* sMessage = sqlite3_column_text(stmt, 3);

				std::cout << sModule << " " << sTime << " " << sLevel << " " << sMessage << std::endl;
			}
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
	}

	sqlite3_close_v2(sql);
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
