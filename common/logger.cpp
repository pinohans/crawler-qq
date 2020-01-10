#include "stdafx.h"
#include "logger.h"

logger::logger(std::wstring wsDir, void(*Callback)(void))
{
	this->init(wsDir, Callback);
}

logger::logger(std::string sDir, void(*Callback)(void))
{
	this->init(U82WS(sDir), Callback);
}

logger::~logger()
{
}


size_t logger::doLog(std::string sLevel, std::string sMessage)
{
	this->GetNow();
	std::string content = this->Escape(this->sTime) + "," + this->Escape(sLevel) + "," + this->Escape(sMessage) + "\n";
	size_t written = fwrite(content.c_str(), sizeof(CHAR), content.length(), this->fpFile);

	fclose(this->fpFile);
	this->fpFile = NULL;

	if (this->Callback) this->Callback();
	return written;
}

size_t logger::doLog(std::wstring wsLevel, std::wstring wsMessage)
{
	return this->doLog(WS2U8(wsLevel), WS2U8(wsMessage));
}

void logger::init(std::wstring wsDir, void(*Callback)(void))
{
	this->pDir = std::filesystem::path(wsDir);
	this->Callback = Callback;
	std::filesystem::create_directories(this->pDir);

	this->fpFile = NULL;
}

std::string logger::Escape(std::string sStr)
{
	std::string sSearch = "\"";
	std::string sReplace = "\"\"";
	size_t pos = sStr.find(sSearch);

	while (pos != std::string::npos)
	{
		sStr.replace(pos, sSearch.size(), sReplace);
		pos = sStr.find(sSearch, pos + sReplace.size());
	}
	return "\"" + sStr + "\"";
}

void logger::GetNow()
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	CHAR strTime[MAX_PATH];
	WCHAR strDate[MAX_PATH];

	sprintf_s(strTime, u8"%04d-%02d-%02d %02d:%02d:%02d",
		stTime.wYear, stTime.wMonth, stTime.wDay,
		stTime.wHour, stTime.wMinute, stTime.wSecond);
	this->sTime = strTime;
	
	wsprintfW(strDate, L"%04d-%02d-%02d",
		stTime.wYear, stTime.wMonth, stTime.wDay);

	std::filesystem::path pTmpFile = this->pDir / std::filesystem::path(strDate);
	pTmpFile += std::filesystem::path(L".csv");

	if (!this->fpFile)
	{
		this->pFile = pTmpFile;
		_wfopen_s(&this->fpFile, this->pFile.c_str(), L"ab");
	}

	if (pTmpFile != this->pFile)
	{
		this->pFile = pTmpFile;
		fclose(this->fpFile);
		_wfopen_s(&this->fpFile, this->pFile.c_str(), L"ab");
	}
	return;
}
