#include "stdafx.h"


std::wstring GB2WS(std::string sStr)
{
	UINT32 len = MultiByteToWideChar(CP_ACP, 0, sStr.c_str(), -1, NULL, 0);
	LPWSTR str = new WCHAR[len];
	MultiByteToWideChar(CP_ACP, 0, sStr.c_str(), -1, str, len);
	std::wstring ret = str;
	if (str) {
		delete str;
	}
	return ret;
}

std::wstring U82WS(std::string sStr)
{
	UINT32 len = MultiByteToWideChar(CP_UTF8, 0, sStr.c_str(), -1, NULL, 0);
	LPWSTR str = new WCHAR[len];
	MultiByteToWideChar(CP_UTF8, 0, sStr.c_str(), -1, str, len);
	std::wstring ret = str;
	if (str) {
		delete str;
	}
	return ret;
}

std::string WS2U8(std::wstring wStr)
{
	UINT32 len = WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, NULL, 0, NULL, NULL);
	LPSTR str = new CHAR[len];
	WideCharToMultiByte(CP_UTF8, 0, wStr.c_str(), -1, str, len, NULL, NULL);
	std::string ret = str;
	if (str) {
		delete str;
	}
	return ret;
}

std::string WS2GB(std::wstring wStr)
{
	UINT32 len = WideCharToMultiByte(CP_ACP, 0, wStr.c_str(), -1, NULL, 0, NULL, NULL);
	LPSTR str = new CHAR[len];
	WideCharToMultiByte(CP_ACP, 0, wStr.c_str(), -1, str, len, NULL, NULL);
	std::string ret = str;
	if (str) {
		delete str;
	}
	return ret;
}

std::string GB2U8(std::string str)
{
	return WS2U8(GB2WS(str));
}

std::string U82G(std::string str)
{
	return WS2GB(U82WS(str));
}

