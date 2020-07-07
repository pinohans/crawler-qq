// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <stdexcept>
#include <limits>
#include <iostream>
#include <windows.h>
typedef int(__cdecl *Hook_TranslateGroupMsgToMsgPack)(PVOID a1, struct CTXBuffer *a2, __int64 a3, struct ITXMsgPack *a4, struct ITXArray *a5);
typedef int(__cdecl *Hook_TranslatePbMsgToMsgPack)(PVOID a1, PVOID a2, PVOID a3, int a4, int a5);
typedef UINT(__cdecl *Hook_GetSelfUin)();
typedef int(__cdecl *Hook_SaveMsg194)(PVOID a1, PVOID a2, PVOID a3, PVOID a4, PVOID a5, unsigned int a6, unsigned int a7, PVOID a8);

Hook_TranslateGroupMsgToMsgPack TrueTranslateGroupMsgToMsgPack;
Hook_TranslatePbMsgToMsgPack TrueTranslatePbMsgToMsgPack;
Hook_GetSelfUin TrueGetSelfUin;
Hook_SaveMsg194 TrueSaveMsg194;

std::filesystem::path pLogpath;
std::filesystem::path pDatapath;
std::string sUin;

int __cdecl NewTranslateGroupMsgToMsgPack(PVOID a1, struct CTXBuffer *a2, __int64 a3, struct ITXMsgPack *a4, struct ITXArray *a5)
{
	BYTE bPreFun = *(BYTE*)((DWORD)a1 - 240);
	// 跳过历史消息，直接处理新消息
	if (bPreFun != 0x3c) {
		return TrueTranslateGroupMsgToMsgPack(a1, a2, a3, a4, a5);
	}
	CHAR* bufMsg = *(*(CHAR***)a1 + 2);
	DWORD uiLen = *(*(DWORD**)a1 + 3);
	std::string sMsg = "";
	for (int i = 0; i < uiLen; i++)sMsg += bufMsg[i];
	msg mMsg = msg(sMsg);
	// sUin
	mMsg.sUin = sUin;

	// sNickname
	// sQQ
	std::string sNicknameQQ = WS2U8((wchar_t*) *((DWORD *)a1 - 13));
	mMsg.sNickname = sNicknameQQ.substr(0, sNicknameQQ.find('('));
	mMsg.sQQ = sNicknameQQ.erase(sNicknameQQ.length() - 1).substr(sNicknameQQ.find('(') + 1);

	// sGroupid
	DWORD uiGroupid = std::stoul(WS2U8((wchar_t*)(*((DWORD *)a1 - 21))));
	if (uiGroupid > 2080000000ul)uiGroupid -= 2080000000ul;
	mMsg.sGroupid = std::to_string(uiGroupid);

	// sGroupname
	mMsg.sGroupname = WS2U8((wchar_t*)((*((DWORD *)a1 - 34)) &(0xffffffff ^ 0x80000000)));

	mMsg.Send();
	return TrueTranslateGroupMsgToMsgPack(a1, a2, a3, a4, a5);
}


int __cdecl NewTranslatePbMsgToMsgPack(PVOID a1, PVOID a2, PVOID a3, int a4, int a5)
{

	DWORD uiEbpAddr;
	_asm mov uiEbpAddr, ebp;

	ITXData txdMsg(*(PVOID*)(*(DWORD*)uiEbpAddr - 0xc));

	DWORD value;
	msg mMsg = msg();
	// sFontname
	mMsg.sFontname = "";
	// sUin
	mMsg.sUin = sUin;
	// sContent
	value = txdMsg.get("msg_body");
	if (value != (DWORD)-1)
	{
		ITXData msg_body((PVOID)value);
		value = msg_body.get("rich_text");
		if (value != (DWORD)-1)
		{
			ITXData rich_text((PVOID)value);
			value = rich_text.get("elems");
			if (value != (DWORD)-1)
			{
				ITXData msg_text((PVOID)((DWORD**)value)[3][0]);
				value = msg_text.get("text");
				if (value != (DWORD)-1)
				{
					ITXData text((PVOID)value);
					value = text.get("str");
					if (value != (DWORD)-1)
					{
						mMsg.sContent = (CHAR*)value;
					}
					else goto END;
				}
				else goto END;
			}
			else goto END;
		}
		else goto END;
	}
	else goto END;

	value = txdMsg.get("msg_head");
	if (value != (DWORD)-1)
	{
		ITXData msg_head((PVOID)value);
		value = msg_head.get("msg_type");
		// 跳过历史消息，直接处理新消息
		if (value != 0x52)
			goto END;

		// sQQ
		value = msg_head.get("from_uin");
		if (value != (DWORD)-1)
			mMsg.sQQ = std::to_string(*(DWORD*)value);

		value = msg_head.get("group_info");
		if (value != (DWORD)-1)
		{
			ITXData group_info((PVOID)value);
			// sNickname
			value = group_info.get("group_card");
			if (value != (DWORD)-1)
				mMsg.sNickname = (CHAR*)value;

			// sGroupid
			value = group_info.get("group_code");
			if (value != (DWORD)-1)
				mMsg.sGroupid = std::to_string(*(DWORD*)value);

		}
		value = msg_head.get("msg_time");
		if (value != (DWORD)-1)
			mMsg.sCrawltime = std::to_string(value);
		else
			mMsg.sCrawltime = std::to_string((DWORD)time(0));

	}

	mMsg.bDone = mMsg.bLongtext = TRUE;

	mMsg.Send();
END:
	return TrueTranslatePbMsgToMsgPack(a1, a2, a3, a4, a5);
}

int __cdecl NewSaveMsg194(PVOID a1, PVOID a2, PVOID a3, PVOID a4, PVOID a5, unsigned int a6, unsigned int a7, PVOID a8)
{
	ITXData txdMsg(a8);
	DWORD value;
	value = txdMsg.get("MSGBOX_bsPreviewMsgText");
	msg mMsg = msg();

	if (value != (DWORD)-1)
	{
		mMsg.sContent = WS2U8((wchar_t*)value);
		mMsg.sType = 2;
		mMsg.sUin = sUin;
		value = txdMsg.get("dwSenderUin");
		if (value != (DWORD)-1)
		{
			mMsg.sQQ = std::to_string(value);
		}
		value = txdMsg.get("strRecieverShowName");
		if (value != (DWORD)-1)
		{
			mMsg.sGroupname = WS2U8((wchar_t*)value);
		}
		value = txdMsg.get("dwTime");
		if (value != (DWORD)-1)
		{
			mMsg.sCrawltime = std::to_string(value);
		}

		mMsg.bDone = TRUE;
		mMsg.Send();
	}
	return TrueSaveMsg194(a1, a2, a3, a4, a5, a6, a7, a8);
}

BOOLEAN Hook()
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)TrueTranslateGroupMsgToMsgPack, NewTranslateGroupMsgToMsgPack);
	DetourAttach(&(PVOID&)TrueTranslatePbMsgToMsgPack, NewTranslatePbMsgToMsgPack);
	DetourAttach(&(PVOID&)TrueSaveMsg194, NewSaveMsg194);
	DetourTransactionCommit();

	return TRUE;
}

BOOLEAN Unhook()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)TrueTranslateGroupMsgToMsgPack, NewTranslateGroupMsgToMsgPack);
	DetourDetach(&(PVOID&)TrueTranslatePbMsgToMsgPack, NewTranslatePbMsgToMsgPack);
	DetourDetach(&(PVOID&)TrueSaveMsg194, NewSaveMsg194);
	DetourTransactionCommit();

	return TRUE;
}

void Init(HMODULE hModule)
{
	WCHAR lpFilename[MAX_PATH];
	GetModuleFileNameW(hModule, lpFilename, MAX_PATH);
	pLogpath = std::filesystem::path(lpFilename).remove_filename() / std::filesystem::path("data");

	hModule = GetModuleHandle(TEXT("KernelUtil.dll"));
	TrueTranslateGroupMsgToMsgPack = (Hook_TranslateGroupMsgToMsgPack)GetProcAddress(hModule, "?TranslateGroupMsgToMsgPack@Msg@Util@@YAHABVCTXBuffer@@_JPAUITXMsgPack@@PAUITXArray@@PAUITXData@@@Z");

	if (!TrueTranslateGroupMsgToMsgPack) {
		return;
	}

	TrueTranslatePbMsgToMsgPack = (Hook_TranslatePbMsgToMsgPack)GetProcAddress(hModule, "?TranslatePbMsgToMsgPack@Msg@Util@@YAHPAUITXData@@PAUITXMsgPack@@PAUITXArray@@HH@Z");

	if (!TrueTranslatePbMsgToMsgPack) {
		return;
	}

	TrueGetSelfUin = (Hook_GetSelfUin)GetProcAddress(hModule, "?GetSelfUin@Contact@Util@@YAKXZ");

	if (!TrueGetSelfUin) {
		return;
	}

	TrueSaveMsg194 = (Hook_SaveMsg194)GetProcAddress(hModule, "?SaveMsg194@Msg@Util@@YAHPB_W000KKPAUITXMsgPack@@PAUITXData@@PAUITXCallback@@@Z");

	if (!TrueSaveMsg194) {
		return;
	}

	std::string sPid = std::to_string(GetCurrentProcessId());
	sUin = std::to_string(TrueGetSelfUin());
	troycrawler::init(pLogpath, sUin);
	troycrawler::config::add("pid", sUin + "/" + sPid);
}

void Quit()
{
	std::map<int, std::string> mPid = troycrawler::config::get("pid");
	for (std::map<int, std::string>::iterator it = mPid.begin(); it != mPid.end(); ++it)
	{
		std::string sPid = it->second;
		std::string module = sPid.substr(0, sPid.find('/'));
		if (module == sUin)
		{
			troycrawler::config::del(it->first);
			return;
		}
	}
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		Init(hModule);
		Hook();
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		Unhook();
		Quit();
        break;
    }
    return TRUE;
}

