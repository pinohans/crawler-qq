#pragma once
#include "logger.h"
#include "httoIO.h"
class msg
{
public:
	msg(logger* log = NULL);
	msg(std::string sMsg, logger* log = NULL);
	~msg();

	std::string sFontname;
	std::string sUin;
	std::string sContent;
	std::string sQQ;
	std::string sNickname;
	std::string sGroupid;
	std::string sGroupname;
	std::string sCrawltime;
	bool bLongtext;
	bool bDone;

	BOOL Send();
	httpIO* http;
protected:
	std::string sMsg;
	void Parse();
	logger *log;
public:
	std::string Jsondump();
};

