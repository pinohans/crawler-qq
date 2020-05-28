// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"

typedef int(__cdecl *Hook_TranslateGroupMsgToMsgPack)(PVOID a1, struct CTXBuffer *a2, __int64 a3, struct ITXMsgPack *a4, struct ITXArray *a5);
typedef int(__cdecl *Hook_TranslatePbMsgToMsgPack)(PVOID a1, PVOID a2, PVOID a3, int a4, int a5);
typedef UINT(__cdecl *Hook_GetSelfUin)();

Hook_TranslateGroupMsgToMsgPack TrueTranslateGroupMsgToMsgPack;
Hook_TranslatePbMsgToMsgPack TrueTranslatePbMsgToMsgPack;
Hook_GetSelfUin TrueGetSelfUin;
logger *mylog;
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
	msg mMsg(sMsg, mylog);
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
	ITXData txdMsgHead((PVOID)txdMsg.v[0].uiValue);

	// 跳过历史消息，直接处理新消息
	if (txdMsgHead.v[1].uiValue != 0x52) {
		return TrueTranslatePbMsgToMsgPack(a1, a2, a3, a4, a5);
	}
	ITXData txdGroupInfo((PVOID)txdMsgHead.v[5].uiValue);
	ITXData txdMsgBody((PVOID)txdMsg.v[1].uiValue);
	ITXData txdRichText((PVOID)txdMsgBody.v[0].uiValue);

	ITXData txdMsgtext((PVOID)((DWORD**)txdRichText.v[0].uiValue)[3][0]);
	ITXData txdText((PVOID)txdMsgtext.v[0].uiValue);


	msg mMsg(mylog);

	mMsg.sContent = (CHAR*)txdText.v[0].uiValue;


	mMsg.sFontname = "";
	mMsg.sCrawltime = std::to_string(txdMsgHead.v[3].uiValue);

	// sUin
	mMsg.sUin = sUin;



	// sNickname
	// sQQ
	mMsg.sNickname = (CHAR*)txdGroupInfo.v[1].uiValue;
	mMsg.sQQ = std::to_string(*(DWORD*)txdMsgHead.v[0].uiValue);

	// sGroupid
	mMsg.sGroupid = std::to_string(*(DWORD*)txdGroupInfo.v[0].uiValue);

	// sGroupname
	mMsg.sGroupname = "";

	mMsg.bDone = mMsg.bLongtext = TRUE;

	mMsg.Send();
	return TrueTranslatePbMsgToMsgPack(a1, a2, a3, a4, a5);
}


BOOLEAN Hook()
{
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)TrueTranslateGroupMsgToMsgPack, NewTranslateGroupMsgToMsgPack);
	DetourAttach(&(PVOID&)TrueTranslatePbMsgToMsgPack, NewTranslatePbMsgToMsgPack);
	DetourTransactionCommit();

	return TRUE;
}

BOOLEAN Unhook()
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)TrueTranslateGroupMsgToMsgPack, NewTranslateGroupMsgToMsgPack);
	DetourDetach(&(PVOID&)TrueTranslatePbMsgToMsgPack, NewTranslatePbMsgToMsgPack);
	DetourTransactionCommit();

	return TRUE;
}

void Init(HMODULE hModule)
{
	WCHAR lpFilename[MAX_PATH];
	GetModuleFileNameW(hModule, lpFilename, MAX_PATH);
	pLogpath = std::filesystem::path(lpFilename).remove_filename() / std::filesystem::path("data");
	logger mainLog = logger(pLogpath, "main");
	std::string sPid = std::to_string(GetCurrentProcessId());

	hModule = GetModuleHandle(TEXT("KernelUtil.dll"));
	mainLog.doLog("debug", sPid + u8"发现KernelUtil.dll句柄");
	TrueTranslateGroupMsgToMsgPack = (Hook_TranslateGroupMsgToMsgPack)GetProcAddress(hModule, "?TranslateGroupMsgToMsgPack@Msg@Util@@YAHABVCTXBuffer@@_JPAUITXMsgPack@@PAUITXArray@@PAUITXData@@@Z");

	if (!TrueTranslateGroupMsgToMsgPack) {
		mainLog.doLog("error", sPid + u8"TrueTranslateGroupMsgToMsgPack获取地址失败");
		return;
	}

	TrueTranslatePbMsgToMsgPack = (Hook_TranslatePbMsgToMsgPack)GetProcAddress(hModule, "?TranslatePbMsgToMsgPack@Msg@Util@@YAHPAUITXData@@PAUITXMsgPack@@PAUITXArray@@HH@Z");

	if (!TrueTranslatePbMsgToMsgPack) {
		mainLog.doLog("error", sPid + u8"TrueTranslatePbMsgToMsgPack获取地址失败");
		return;
	}

	TrueGetSelfUin = (Hook_GetSelfUin)GetProcAddress(hModule, "?GetSelfUin@Contact@Util@@YAKXZ");

	if (!TrueGetSelfUin) {
		mainLog.doLog("error", sPid + u8"TrueGetSelfUin获取地址失败");
		return;
	}
	sUin = std::to_string(TrueGetSelfUin());
	mylog = new logger(pLogpath, sUin);
	if (!std::filesystem::exists(pLogpath / sUin)) std::filesystem::create_directories(pLogpath / sUin);

	mylog->doConfig("pid", sPid);
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
		mylog->doConfig("pid", "");
		if (mylog)delete mylog;
		mylog = NULL;
        break;
    }
    return TRUE;
}

