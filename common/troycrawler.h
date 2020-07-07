#pragma once
namespace troycrawler
{
	extern std::string sModule;
	extern std::filesystem::path pFilepath;
	BOOL init(std::filesystem::path _pFilepath, std::string _sModule);
	BOOL quit();
};