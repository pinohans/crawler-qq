#pragma once

#include <comdef.h>
#include <Wbemidl.h>
#include <vector>
#include "stdafx.h"
#include <comdef.h>
#pragma comment(lib, "wbemuuid.lib")

class EventSink : public IWbemObjectSink
{
	LONG m_lRef;
	bool bDone;

public:
	EventSink() { m_lRef = 0; }
	~EventSink() { bDone = true; }

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return InterlockedIncrement(&m_lRef);
	}

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		LONG IRef = InterlockedDecrement(&m_lRef);
		if (IRef == 0)
			delete this;
		return IRef;
	}

	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppv)
	{
		if (riid == IID_IUnknown || riid == IID_IWbemObjectSink)
		{
			*ppv = (IWbemObjectSink*)this;
			AddRef();
			return WBEM_S_NO_ERROR;
		}
		else return E_NOINTERFACE;
	}

	virtual HRESULT STDMETHODCALLTYPE Indicate(
		LONG lObjectCount,
		IWbemClassObject __RPC_FAR *__RPC_FAR *apObjArray
	);
	virtual HRESULT STDMETHODCALLTYPE SetStatus(LONG IFlags, HRESULT hResult, BSTR strParam, IWbemClassObject __RPC_FAR *pObjParam) {
		return WBEM_S_NO_ERROR;
	}
};
#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

#ifdef INJECTION_DLL
#define INJECTIONAPI __declspec(dllexport)
#else
#define INJECTIONAPI __declspec(dllimport)
#endif

INJECTIONAPI int start();
INJECTIONAPI int stop();
INJECTIONAPI void inject();
#ifdef __cplusplus
}
#endif
