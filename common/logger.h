#pragma once
#include <filesystem>
#include "convert.h"

class logger
{
public:
	void(*Callback)(void);

	logger(std::wstring wsDir, void(*Callback)(void) = NULL);
	logger(std::string sDir, void(*Callback)(void) = NULL);
	~logger();


	size_t doLog(std::string sLevel, std::string sMessage);
	size_t doLog(std::wstring wsLevel, std::wstring wsMessage);

protected:
	std::filesystem::path pDir;
	FILE* fpFile;

	std::filesystem::path pFile;
	std::string sTime;


	void init(std::wstring wsDir, void(*Callback)(void));

	std::string Escape(std::string wsStr);

	void GetNow();
};
