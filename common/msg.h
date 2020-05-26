#pragma once

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

