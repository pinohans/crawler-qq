#pragma once

struct ITXDataItem
{
	ITXDataItem(){}
	ITXDataItem(std::string _sName, DWORD _uiValue) { this->sName = _sName, this->uiValue = _uiValue; }
	std::string sName;
	DWORD uiValue;
};

struct ITXDataItemType
{
	CHAR* strName;
	BYTE btType;
};

class ITXData
{
public:
	ITXData(PVOID pAddr);
	~ITXData();

	std::vector<ITXDataItem> v;
	DWORD get(std::string _sName);
};

