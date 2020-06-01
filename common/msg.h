#pragma once

class msg
{
public:
	msg(logger* log = NULL);
	msg(std::string sMsg, logger* log = NULL);
	~msg();

	std::string sFontname="";
	std::string sUin="0";
	std::string sContent="None";
	std::string sQQ="0";
	std::string sNickname= "None";
	std::string sGroupid="0";
	std::string sGroupname="None";
	std::string sCrawltime="0";
	std::string sSource = "qq";
	std::string sTitle = "PC";
	bool bLongtext;
	bool bDone;

	BOOL Send();
	httpIO* http;
	sqlite3 * sql = NULL;
protected:
	std::string sMsg;
	void Parse();
	logger *log;
public:
	std::string Jsondump();
};

