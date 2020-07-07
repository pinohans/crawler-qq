#pragma once

class msg
{
public:
	msg();
	msg(std::string sMsg);
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
	std::string sTitle = "pc";
	std::string sType = "1";
	bool bLongtext;
	bool bDone;

	BOOL Send();
protected:
	std::string sMsg;
	void Parse();
public:
	std::string Jsondump();
};

