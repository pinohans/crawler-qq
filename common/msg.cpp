#include "stdafx.h"


msg::msg()
{
}

msg::msg(std::string sMsg)
{
	this->sMsg = sMsg;

	// sFontname
	// sContent
	// sCrawltime
	// bLongtext
	// bDone
	this->Parse();
}


msg::~msg()
{
}

void msg::Parse()
{
	std::string sTmpmsg = this->sMsg;
	std::string sSubtmpmsg;
	DWORD uiTime = 0u;
	DWORD uiFontlen = 0u;
	DWORD uiLen = 0u;
	DWORD uiSublen = 0u;
	BYTE btType;
	BYTE btSubtype;

	// Magic
	if (sTmpmsg.length() < 0x12) goto FAILED;
	sTmpmsg = sTmpmsg.substr(0x12);

	// Time
	if (sTmpmsg.length() < 0x8) goto FAILED;
	for (int i = 0; i < 4; i++)uiTime = uiTime * 0x100u + (BYTE)sTmpmsg[i];
	this->sCrawltime = std::to_string(uiTime);
	sTmpmsg = sTmpmsg.substr(0x8);

	// Unknown1
	if (sTmpmsg.length() < 0x8) goto FAILED;
	sTmpmsg = sTmpmsg.substr(0x8);

	// Font
	if (sTmpmsg.length() < 0x2) goto FAILED;
	for (int i = 0; i < 2; i++) uiFontlen = uiFontlen * 0x100u + (BYTE)sTmpmsg[i];
	sTmpmsg = sTmpmsg.substr(0x2);

	if (sTmpmsg.length() < uiFontlen) goto FAILED;
	this->sFontname = sTmpmsg.substr(0, uiFontlen);
	sTmpmsg = sTmpmsg.substr(uiFontlen);

	// Unknown2
	if (sTmpmsg.length() < 0x2) goto FAILED;
	sTmpmsg = sTmpmsg.substr(0x2);

	// Message
	this->sContent = "";
	while (sTmpmsg.length()) {
		btType = (BYTE)sTmpmsg[0];
		sTmpmsg = sTmpmsg.substr(0x1);

		uiLen = 0u;
		if (sTmpmsg.length() < 0x2) goto FAILED;
		for (int i = 0; i < 2; i++) uiLen = uiLen * 0x100u + (BYTE)sTmpmsg[i];
		sTmpmsg = sTmpmsg.substr(0x2);

		if (sTmpmsg.length() < uiLen) goto FAILED;
		sSubtmpmsg = sTmpmsg.substr(0, uiLen);
		sTmpmsg = sTmpmsg.substr(uiLen);

		switch (btType) {
		case 0x01:
			while (sSubtmpmsg.length()) {
				btSubtype = (BYTE)sSubtmpmsg[0];
				sSubtmpmsg = sSubtmpmsg.substr(0x1);

				uiSublen = 0u;
				if (sSubtmpmsg.length() < 0x2) goto FAILED;
				for (int i = 0; i < 2; i++) uiSublen = uiSublen * 0x100u + (BYTE)sSubtmpmsg[i];
				sSubtmpmsg = sSubtmpmsg.substr(0x2);

				if(sSubtmpmsg.length() < uiSublen) goto FAILED;
				switch (btSubtype) {
				case 0x01:
					this->sContent += sSubtmpmsg.substr(0, uiSublen);
					break;
				default:
					break;
				}
				sSubtmpmsg = sSubtmpmsg.substr(uiSublen);
			}
			break;
		case 0x02:
			this->sContent += WS2U8(L"[表情]");
			break;
		default:
			break;
		}
	}

	if (this->sContent.length())
	{
		this->bLongtext = this->sContent == WS2U8(L"你的QQ暂不支持查看[转发多条消息]，请期待后续版本。");
		this->bDone = TRUE ^ this->bLongtext;
		return;
	}
FAILED:
	this->bDone = FALSE;
	return;
}



BOOL msg::Send()
{
	if (this->bDone)
	{
		std::string sMessage = this->Jsondump();
		troycrawler::log::debug(sMessage);
		std::map<int,std::string> url = troycrawler::config::get("url");
		for (std::map<int, std::string>::iterator it = url.begin(); it != url.end(); ++it)
		{
			troycrawler::http::post(it->second, sMessage);
		}
		return TRUE;
	}
	return FALSE;
}



std::string msg::Jsondump()
{
	// TODO: 在此处添加实现代码.

	json jMessage,jData;
	jMessage["status"] = "0";
	jMessage["type"] = "1";
	jData["uin"] = this->sUin;
	jData["content"] = this->sContent;
	jData["qq"] = this->sQQ;
	jData["nickname"] = this->sNickname;
	jData["groupid"] = this->sGroupid;
	jData["groupname"] = this->sGroupname;
	jData["crawltime"] = this->sCrawltime;
	jData["title"] = this->sTitle;
	jData["source"] = this->sSource;
	jData["type"] = this->sType;
	jData["status"] = "0";
	jMessage["data"] = jData;
	
	return jMessage.dump();
}
