#include "stdafx.h"
#include "troycrawler.h"

BOOL troycrawler::init(std::filesystem::path _pFilepath, std::string _sModule)
{
	sModule = _sModule;
	pFilepath = _pFilepath;
	std::filesystem::create_directories(pFilepath);
	return log::init() && config::init() && http::init();
}

BOOL troycrawler::quit()
{
	return http::quit();
}

std::string troycrawler::sModule;
std::filesystem::path troycrawler::pFilepath;
