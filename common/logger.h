#pragma once

class logger
{
public:


	logger(std::filesystem::path path, std::string sModule);

	~logger();
	sqlite3 *sql = NULL;
	sqlite3 *sql_log = NULL;

	BOOL doLog(std::string sLevel, std::string sMessage);
	BOOL doConfig(std::string sKey, std::string sValue, BOOL bUpdate = TRUE);
	BOOL deleteConfig(std::string sKey, int id = -1);

	std::filesystem::path path;
	std::string sModule;
};
