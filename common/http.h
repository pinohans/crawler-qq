#pragma once

namespace troycrawler
{
	namespace http
	{
		BOOL init();
		std::string get(std::string _sUrl);
		std::string post(std::string _sUrl, std::string _sData);
		BOOL quit();
	};
};