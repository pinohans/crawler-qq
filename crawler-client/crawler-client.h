﻿
// crawler-client.h: PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号


// CcrawlerclientApp:
// 有关此类的实现，请参阅 crawler-client.cpp
//

class CcrawlerclientApp : public CWinApp
{
public:
	CcrawlerclientApp();
	httpIO* http;
	logger* log;
	HMODULE hInjectModule;
	csv::Reader* cr;
	std::filesystem::path pLogpath;
	HANDLE hInjectStop;

// 重写
public:
	virtual BOOL InitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
};

extern CcrawlerclientApp theApp;
