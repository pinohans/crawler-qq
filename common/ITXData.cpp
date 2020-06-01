#include "stdafx.h"


ITXData::ITXData(PVOID pAddr)
{
	this->v.clear();
	BYTE* pITXDataItem = (BYTE*)*((DWORD*)pAddr + 2);
	DWORD uiLen = *(pITXDataItem + 1);
	pITXDataItem += 4;
	CHAR* pITXDataItemType = *(CHAR**)(0x2a2308 + (DWORD)GetModuleHandleW(L"Common.dll"));
	for (DWORD i = 0u; i < uiLen; i++)
	{
		WORD uiOff = *(WORD*)pITXDataItem;
		pITXDataItem += 2;
		DWORD uiValue = 0u;
		ITXDataItemType* pTmpITXDataItemType = (ITXDataItemType *)(pITXDataItemType + 5 * uiOff);
		if (pTmpITXDataItemType->btType <= 0x7)
		{
			DWORD uiLshift = 0;
			do
			{
				uiValue |= (((*pITXDataItem) & 0x7f) << (uiLshift*7));
				uiLshift += 1;
				pITXDataItem += 1;
			}
			while (pITXDataItem[-1] & 0x80);
		}
		else
		{
			uiValue = *(DWORD*)pITXDataItem;
			pITXDataItem += 4;
		}
		this->v.push_back(ITXDataItem(pTmpITXDataItemType->strName, uiValue));
	}
}


ITXData::~ITXData()
{
}

DWORD ITXData::get(std::string _sName)
{
	for (std::vector<ITXDataItem>::iterator it = this->v.begin(); it != this->v.end(); ++it)
	{
		if ((*it).sName == _sName)
		{
			return (*it).uiValue;
		}
	}
	return -1;
}
