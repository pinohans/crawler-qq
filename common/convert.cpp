#include "stdafx.h"
#include "convert.h"
#include "../common/include/openssl/md5.h"


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



std::string Getmd5(std::string filename)
{
	unsigned char c[MD5_DIGEST_LENGTH];
	char buf[3];
	int i;
	FILE *fp = NULL;
	fopen_s(&fp, filename.c_str(), "rb");
	MD5_CTX mdContext;
	int bytes;
	unsigned char data[1024];
	std::string ret = "";

	if (fp == NULL)
		return "";

	MD5_Init(&mdContext);
	while ((bytes = fread(data, 1, 1024, fp)) != 0)
		MD5_Update(&mdContext, data, bytes);
	MD5_Final(c, &mdContext);
	for (i = 0; i < MD5_DIGEST_LENGTH; i++)
	{
		sprintf_s(buf, "%02x\0", c[i]);
		ret += buf;
	}
	fclose(fp);
	return ret;
}