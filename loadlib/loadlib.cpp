﻿// loadlib.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <SDKDDKVer.h>
// Windows 头文件
#include <windows.h>
#include "pch.h"
#include <iostream>
#include <tchar.h>
#include <Wbemidl.h>
#include <vector>
#include <comdef.h>
#include <tlhelp32.h>
#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib,"Kernel32.lib")
using namespace std;
#define RTN_OK 0
#define RTN_USAGE 1
#define RTN_ERROR 13

BOOL SetPrivilege(
	HANDLE hToken,          // token handle
	LPCTSTR Privilege,      // Privilege to enable/disable
	BOOL bEnablePrivilege   // TRUE to enable.  FALSE to disable
)
{
	TOKEN_PRIVILEGES tp;
	LUID luid;
	TOKEN_PRIVILEGES tpPrevious;
	DWORD cbPrevious = sizeof(TOKEN_PRIVILEGES);

	if (!LookupPrivilegeValue(NULL, Privilege, &luid)) return FALSE;

	// 
	// first pass.  get current privilege setting
	// 
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
	tp.Privileges[0].Attributes = 0;

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tp,
		sizeof(TOKEN_PRIVILEGES),
		&tpPrevious,
		&cbPrevious
	);

	if (GetLastError() != ERROR_SUCCESS) return FALSE;

	// 
	// second pass.  set privilege based on previous setting
	// 
	tpPrevious.PrivilegeCount = 1;
	tpPrevious.Privileges[0].Luid = luid;

	if (bEnablePrivilege) {
		tpPrevious.Privileges[0].Attributes |= (SE_PRIVILEGE_ENABLED);
	}
	else {
		tpPrevious.Privileges[0].Attributes ^= (SE_PRIVILEGE_ENABLED &
			tpPrevious.Privileges[0].Attributes);
	}

	AdjustTokenPrivileges(
		hToken,
		FALSE,
		&tpPrevious,
		cbPrevious,
		NULL,
		NULL
	);

	if (GetLastError() != ERROR_SUCCESS) return FALSE;

	return TRUE;
}

BOOL LoadLib(LPCTSTR sModule, DWORD dwID)
{

	TCHAR szModule[MAX_PATH];
	GetFullPathName(sModule, MAX_PATH, szModule, NULL);

	HANDLE hToken;
	int dwRetVal = RTN_OK; // assume success from main()

	if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
	{
		if (GetLastError() == ERROR_NO_TOKEN)
		{
			if (!ImpersonateSelf(SecurityImpersonation))
				return RTN_ERROR;

			if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken)) {
				return RTN_ERROR;
			}
		}
		else
			return RTN_ERROR;
	}
	cout << 1 << endl;
	// enable SeDebugPrivilege
	if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE))
	{
		CloseHandle(hToken);
		return RTN_ERROR;
	}
	cout << 2 << endl;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwID);//打开进程
	if (!hProcess) {
		CloseHandle(hToken);
		return FALSE;
	}
	cout << 3 << endl;

	int cByte = (_tcslen(szModule) + 1) * sizeof(TCHAR);
	LPVOID pAddr = VirtualAllocEx(hProcess, NULL, cByte, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);//申请内存

	if (!pAddr || !WriteProcessMemory(hProcess, pAddr, szModule, cByte, NULL))//写入dll地址
	{
		DWORD e = GetLastError();
		cout << "error " << e << endl;
		VirtualFreeEx(hProcess, pAddr, cByte, MEM_RELEASE);
		CloseHandle(hProcess);
		CloseHandle(hToken);
		return FALSE;
	}
	cout << 4 << endl;

#ifdef _UNICODE  
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
#else
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")), "LoadLibraryW");
#endif//宽A和U定义 

	if (!pfnStartAddr)
	{
		VirtualFreeEx(hProcess, pAddr, cByte, MEM_RELEASE);
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return FALSE;
	}
	DWORD dwThreadID = 0;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pfnStartAddr, pAddr, 0, &dwThreadID);
	if (!hRemoteThread)
	{
		VirtualFreeEx(hProcess, pAddr, cByte, MEM_RELEASE);
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return FALSE;
	}
	VirtualFreeEx(hProcess, pAddr, cByte, MEM_RELEASE);
	CloseHandle(hToken);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
	return TRUE;

}

BOOL UnloadLib(LPCTSTR sModule, DWORD dwID)
{

	HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwID);
	MODULEENTRY32 me32;
	me32.dwSize = sizeof(MODULEENTRY32);
	HMODULE hModule = NULL;
	if (hModuleSnap == INVALID_HANDLE_VALUE) return FALSE;
	if (!Module32First(hModuleSnap, &me32))
	{
		CloseHandle(hModuleSnap);
		return FALSE;
	}
	cout << 1 << endl;

	do
	{
		if (wcscmp(sModule, me32.szModule) == 0)
		{
			hModule = me32.hModule;
		}
	} while (Module32Next(hModuleSnap, &me32));
	CloseHandle(hModuleSnap);

	if (!hModule) return FALSE;
	cout << 2 << endl;

	HANDLE hToken;
	int dwRetVal = RTN_OK; // assume success from main()

	if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken))
	{
		if (GetLastError() == ERROR_NO_TOKEN)
		{
			if (!ImpersonateSelf(SecurityImpersonation))
				return RTN_ERROR;

			if (!OpenThreadToken(GetCurrentThread(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, FALSE, &hToken)) {
				return RTN_ERROR;
			}
		}
		else
			return RTN_ERROR;
	}
	cout << 3 << endl;

	// enable SeDebugPrivilege
	if (!SetPrivilege(hToken, SE_DEBUG_NAME, TRUE))
	{
		CloseHandle(hToken);
		return RTN_ERROR;
	}
	cout << 4 << endl;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwID);//打开进程
	if (!hProcess) {
		CloseHandle(hToken);
		return FALSE;
	}

	cout << 5 << endl;

#ifdef _UNICODE  
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")), "FreeLibrary");
#else
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(_T("Kernel32")), "FreeLibrary");
#endif//宽A和U定义 

	if (!pfnStartAddr)
	{
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return FALSE;
	}
	DWORD dwThreadID = 0;
	HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, pfnStartAddr, hModule, 0, &dwThreadID);
	if (!hRemoteThread)
	{
		CloseHandle(hToken);
		CloseHandle(hProcess);
		return FALSE;
	}

	CloseHandle(hToken);
	CloseHandle(hRemoteThread);
	CloseHandle(hProcess);
	return TRUE;

}
int main(int argc, char *argv[])
{
	if (strcmp(argv[1], "-u") == 0)
	{
		UnloadLib(TEXT("crawler.dll"), atoi(argv[2]));
	}
	else
	{
		LoadLib(TEXT("crawler.dll"), atoi(argv[1]));
	}
}

