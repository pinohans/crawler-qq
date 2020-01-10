#include "stdafx.h"
#define INJECTION_DLL
#include "injection.h"

#include <tchar.h>
#include <tlhelp32.h>
#include <string>


IWbemObjectSink* pStubSink;
EventSink* pSink;
IUnknown* pStubUnk;
IUnsecuredApartment* pUnsecApp;
IWbemLocator *pLoc;
IWbemServices *pSvc;
std::wstring GetProcessName(DWORD dwPid)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (hProcessSnap == INVALID_HANDLE_VALUE) return L"";
	if (Process32First(hProcessSnap, &pe32)) {
		do {
			if (pe32.th32ProcessID == dwPid) {
				CloseHandle(hProcessSnap);
				return pe32.szExeFile;
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);
	return L"";
}

DWORD GetPPid(DWORD dwPid)
{
	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (hProcessSnap == INVALID_HANDLE_VALUE) return -1;
	if (Process32First(hProcessSnap, &pe32)) {
		do {
			if (pe32.th32ProcessID == dwPid) {
				CloseHandle(hProcessSnap);
				return pe32.th32ParentProcessID;
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);
	return -1;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	DWORD dwPid = *(DWORD*)lpParam;
	delete (DWORD*)lpParam;
	

	while (TRUE)
	{
		LPCWSTR moduleName = L"QQGame.dll";
		HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPid);
		MODULEENTRY32 me32;
		me32.dwSize = sizeof(MODULEENTRY32);

		if (hModuleSnap == INVALID_HANDLE_VALUE) return 1;
		if (!Module32First(hModuleSnap, &me32))
		{
			CloseHandle(hModuleSnap);
			continue;
		}

		do
		{
			if (wcscmp(moduleName, me32.szModule) == 0)
			{
				STARTUPINFO si;
				PROCESS_INFORMATION pi;

				ZeroMemory(&si, sizeof(si));
				si.cb = sizeof(si);
				ZeroMemory(&pi, sizeof(pi));
				CreateProcess(L"loadlib.exe", (LPWSTR)(L" " + std::to_wstring(dwPid)).c_str(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
				WaitForSingleObject(pi.hProcess, INFINITE);
				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
				CloseHandle(hModuleSnap);
				return 1;
			}
		} while (Module32Next(hModuleSnap, &me32));
		CloseHandle(hModuleSnap);

		HANDLE hEvent = OpenEventW(EVENT_ALL_ACCESS, FALSE, L"INJECT_STOP");
		if (WaitForSingleObject(hEvent, 1000) == WAIT_TIMEOUT)
		{
			CloseHandle(hEvent);
			continue;
		}
		else
		{
			CloseHandle(hEvent);
			return 1;
		}
	}
}

int start()
{
	// TODO: 在此处添加实现代码.

	HRESULT hres;

	// Step 1: --------------------------------------------------
	// Initialize COM. ------------------------------------------

	hres = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hres))
	{
		return 1;                  // Program has failed.
	}

	// Step 2: --------------------------------------------------
	// Set general COM security levels --------------------------
	
	hres = CoInitializeSecurity(
		NULL,
		-1,                          // COM negotiates service
		NULL,                        // Authentication services
		NULL,                        // Reserved
		RPC_C_AUTHN_LEVEL_DEFAULT,   // Default authentication 
		RPC_C_IMP_LEVEL_IMPERSONATE, // Default Impersonation  
		NULL,                        // Authentication info
		EOAC_NONE,                   // Additional capabilities 
		NULL                         // Reserved
	);


	if (hres != RPC_E_TOO_LATE && FAILED(hres))
	{
		CoUninitialize();
		return 1;                      // Program has failed.
	}

	// Step 3: ---------------------------------------------------
	// Obtain the initial locator to WMI -------------------------


	hres = CoCreateInstance(
		CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator, (LPVOID *)&pLoc);

	if (FAILED(hres))
	{
		CoUninitialize();
		return 1;                 // Program has failed.
	}

	// Step 4: ---------------------------------------------------
	// Connect to WMI through the IWbemLocator::ConnectServer method


	// Connect to the local root\cimv2 namespace
	// and obtain pointer pSvc to make IWbemServices calls.
	hres = pLoc->ConnectServer(
		_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc
	);

	if (FAILED(hres))
	{
		pLoc->Release();
		CoUninitialize();
		return 1;                // Program has failed.
	}


	// Step 5: --------------------------------------------------
	// Set security levels on the proxy -------------------------

	hres = CoSetProxyBlanket(
		pSvc,                        // Indicates the proxy to set
		RPC_C_AUTHN_WINNT,           // RPC_C_AUTHN_xxx 
		RPC_C_AUTHZ_NONE,            // RPC_C_AUTHZ_xxx 
		NULL,                        // Server principal name 
		RPC_C_AUTHN_LEVEL_CALL,      // RPC_C_AUTHN_LEVEL_xxx 
		RPC_C_IMP_LEVEL_IMPERSONATE, // RPC_C_IMP_LEVEL_xxx
		NULL,                        // client identity
		EOAC_NONE                    // proxy capabilities 
	);

	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
		return 1;               // Program has failed.
	}

	// Step 6: -------------------------------------------------
	// Receive event notifications -----------------------------

	// Use an unsecured apartment for security

	hres = CoCreateInstance(CLSID_UnsecuredApartment, NULL,
		CLSCTX_LOCAL_SERVER, IID_IUnsecuredApartment,
		(void**)&pUnsecApp);

	pSink = new EventSink;
	pSink->AddRef();

	pUnsecApp->CreateObjectStub(pSink, &pStubUnk);

	pStubUnk->QueryInterface(IID_IWbemObjectSink,
		(void **)&pStubSink);

	// The ExecNotificationQueryAsync method will call
	// The EventQuery::Indicate method when an event occurs
	hres = pSvc->ExecNotificationQueryAsync(
		_bstr_t("WQL"),
		_bstr_t("SELECT * "
			"FROM __InstanceCreationEvent WITHIN 1 "
			"WHERE TargetInstance ISA 'Win32_Process' and TargetInstance.Name = 'qq.exe'"),
		WBEM_FLAG_SEND_STATUS,
		NULL,
		pStubSink);

	// Check for errors.
	if (FAILED(hres))
	{
		pSvc->Release();
		pLoc->Release();
		pUnsecApp->Release();
		pStubUnk->Release();
		pSink->Release();
		pStubSink->Release();
		CoUninitialize();
		return 1;
	}

	inject();

	return 0; // Program successfully completed.

}
void inject()
{

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (hProcessSnap == INVALID_HANDLE_VALUE) return;
	if (Process32First(hProcessSnap, &pe32)) {
		do {
			if (wcscmp(pe32.szExeFile, L"QQ.exe") == 0)
			{
				DWORD dwThreadId;
				DWORD* dwProcessId = new DWORD;
				dwProcessId[0] = pe32.th32ProcessID;
				CreateThread(
					NULL,                   // default security attributes
					0,                      // use default stack size  
					MyThreadFunction,       // thread function name
					dwProcessId,          // argument to thread function 
					0,                      // use default creation flags 
					&dwThreadId);   // returns the thread identifier 
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);
}
int stop()
{
	// TODO: 在此处添加实现代码.
	pSvc->CancelAsyncCall(pStubSink);
	pSvc->Release();
	pLoc->Release();
	pUnsecApp->Release();
	pStubUnk->Release();
	pSink->Release();
	pStubSink->Release();
	CoUninitialize();

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (hProcessSnap == INVALID_HANDLE_VALUE) return 0;
	if (Process32First(hProcessSnap, &pe32)) {
		do {
			if (wcscmp(pe32.szExeFile, L"QQ.exe") == 0)
			{
				DWORD dwThreadId;
				DWORD* pArgs = new DWORD;
				pArgs[0] = pe32.th32ProcessID;
				CreateThread(
					NULL,                   // default security attributes
					0,                      // use default stack size  
					MyThreadFunction,       // thread function name
					pArgs,          // argument to thread function 
					0,                      // use default creation flags 
					&dwThreadId);   // returns the thread identifier 
			}
		} while (Process32Next(hProcessSnap, &pe32));
	}
	CloseHandle(hProcessSnap);

	return 0; // Program successfully completed.
}


HRESULT STDMETHODCALLTYPE EventSink::Indicate(
	LONG lObjectCount,
	IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
)
{
	HRESULT hr = S_OK;
	_variant_t vtProp;

	HANDLE*  hThreadArray = new HANDLE[lObjectCount];
	for (int i = 0; i < lObjectCount; i++)
	{

		hr = apObjArray[i]->Get(_bstr_t(L"TargetInstance"), 0, &vtProp, 0, 0);
		if (!FAILED(hr))
		{
			IUnknown* str = vtProp;
			hr = str->QueryInterface(IID_IWbemClassObject, reinterpret_cast<void**>(&apObjArray[i]));
			if (SUCCEEDED(hr))
			{
				_variant_t ProcessId;
				hr = apObjArray[i]->Get(L"ProcessId", 0, &ProcessId, NULL, NULL);
				if (SUCCEEDED(hr))
				{
					DWORD dwThreadId;
					DWORD* dwProcessId = new DWORD;
					dwProcessId[0] = ProcessId.ulVal;
					CreateThread(
						NULL,                   // default security attributes
						0,                      // use default stack size  
						MyThreadFunction,       // thread function name
						dwProcessId,          // argument to thread function 
						0,                      // use default creation flags 
						&dwThreadId);   // returns the thread identifier 

				}
				VariantClear(&ProcessId);
			}
		}
		VariantClear(&vtProp);
		WaitForMultipleObjects(lObjectCount, hThreadArray, TRUE, INFINITE);
		delete hThreadArray;
	}

	return WBEM_S_NO_ERROR;
}
