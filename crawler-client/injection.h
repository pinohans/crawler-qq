#pragma once

#include <comdef.h>
#include <Wbemidl.h>
#include <vector>
#include "stdafx.h"
#include <comdef.h>
#pragma comment(lib, "wbemuuid.lib")

class injection
{
public:
	HANDLE hStopInject;
	HANDLE hInjection;
	INT start();
	INT stop();
	DWORD dwInjectionThreadId;
	BOOL IsStarted();
	injection();
};
