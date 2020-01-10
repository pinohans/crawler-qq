#pragma once
#include "logger.h"
size_t WriteData(void *ptr, size_t size, size_t nmemb, void *stream);

size_t DownloadData(void *ptr, size_t size, size_t nmemb, void *stream);

class httpIO
{
public:
	httpIO(logger* pLog);

	~httpIO();

	std::string Get(std::string sUrl);
	std::string Post(std::string sUrl, std::string sData);
	BOOL Download(std::string sUrl, std::string sFilename);
protected:
	logger* pLog;

public:
	bool SendRabbitmq(std::string sHostname,
		std::string sPort,
		std::string sVhost,
		std::string sUsername,
		std::string sPassword,
		std::string sQueue,
		std::string sExchange,
		std::string sRoutingkey,
		std::string sMessage);
};
