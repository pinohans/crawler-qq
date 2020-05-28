#include "stdafx.h"

logger::logger(std::filesystem::path path, std::string sModule)
{
	this->path = path;
	this->sModule = sModule;
}

logger::~logger()
{
}

std::string GetNow(int level)
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	CHAR strTime[MAX_PATH];
	if (level == 1)
	{
		sprintf_s(strTime, u8"%04d-%02d-%02d",
			stTime.wYear, stTime.wMonth, stTime.wDay);
	}
	else if (level == 2)
	{
		sprintf_s(strTime, u8"%04d-%02d-%02d %02d:%02d:%02d",
			stTime.wYear, stTime.wMonth, stTime.wDay,
			stTime.wHour, stTime.wMinute, stTime.wSecond);
	}
	return strTime;
}


BOOL logger::doLog(std::string sLevel, std::string sMessage)
{
	std::filesystem::path pLogFilename = this->path / this->sModule / (GetNow(1) + ".db");
	if (this->sql && !std::filesystem::path(sqlite3_db_filename(this->sql, NULL)).compare(pLogFilename)) {}
	else
	{

		if (this->sql)
		{
			sqlite3_close_v2(this->sql);
			this->sql = NULL;
		}
		int result = sqlite3_open_v2(WS2U8(pLogFilename.wstring()).c_str(), &this->sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
		if (result != SQLITE_OK)
		{
			this->sql = NULL;
			return FALSE;
		}
	}
	sqlite3_stmt *stmt = NULL;

	int result = sqlite3_prepare_v2(sql, "CREATE TABLE IF NOT EXISTS log( id integer PRIMARY KEY AUTOINCREMENT, time text NOT NULL, level text NOT NULL, message text NOT NULL ); ", -1, &stmt, NULL);

	if (result == SQLITE_OK) {
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		result = sqlite3_prepare_v2(sql, "INSERT INTO log(time, level, message) VALUES(?,?,?);", -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_text(stmt, 1, GetNow(2).c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 2, sLevel.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 3, sMessage.c_str(), -1, SQLITE_STATIC);
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
	}
	return TRUE;
}


BOOL logger::doConfig(std::string sKey, std::string sValue, BOOL bUpdate)
{
	std::filesystem::path pLogFilename = this->path / "config.db";
	if (this->sql_log) {}
	else
	{
		int result = sqlite3_open_v2(WS2U8(pLogFilename.wstring()).c_str(), &this->sql_log, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
		if (result != SQLITE_OK)
		{
			this->sql_log = NULL;
			return FALSE;
		}
	}
	sqlite3_stmt *stmt = NULL;

	int result = sqlite3_prepare_v2(this->sql_log, "CREATE TABLE IF NOT EXISTS config( id integer PRIMARY KEY AUTOINCREMENT, module text NOT NULL, key text NOT NULL, value text NOT NULL ); ", -1, &stmt, NULL);

	if (result == SQLITE_OK) {
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);

		result = sqlite3_prepare_v2(this->sql_log, "SELECT count(*) FROM config WHERE module=? AND key=?;", -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_text(stmt, 1, this->sModule.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 2, sKey.c_str(), -1, SQLITE_STATIC);
			if (sqlite3_step(stmt) == SQLITE_ROW)
			{
				if (sqlite3_column_int(stmt, 0) > 0 && bUpdate)
				{
					sqlite3_finalize(stmt);
					result = sqlite3_prepare_v2(this->sql_log, "UPDATE config SET value=? WHERE module=? AND key=?;", -1, &stmt, NULL);
					if (result == SQLITE_OK) {
						sqlite3_bind_text(stmt, 1, sValue.c_str(), -1, SQLITE_STATIC);
						sqlite3_bind_text(stmt, 2, this->sModule.c_str(), -1, SQLITE_STATIC);
						sqlite3_bind_text(stmt, 3, sKey.c_str(), -1, SQLITE_STATIC);
						sqlite3_step(stmt);
						sqlite3_finalize(stmt);
					}
				}
				else
				{
					sqlite3_finalize(stmt);
					result = sqlite3_prepare_v2(this->sql_log, "INSERT INTO config(module, key, value) VALUES(?,?,?);", -1, &stmt, NULL);
					if (result == SQLITE_OK) {
						sqlite3_bind_text(stmt, 1, this->sModule.c_str(), -1, SQLITE_STATIC);
						sqlite3_bind_text(stmt, 2, sKey.c_str(), -1, SQLITE_STATIC);
						sqlite3_bind_text(stmt, 3, sValue.c_str(), -1, SQLITE_STATIC);
						sqlite3_step(stmt);
						sqlite3_finalize(stmt);
					}
				}
			}
		}

	}
	return TRUE;
}



BOOL logger::deleteConfig(std::string sKey, int id)
{
	std::filesystem::path pLogFilename = this->path / "config.db";
	if (this->sql_log) {}
	else
	{
		int result = sqlite3_open_v2(WS2U8(pLogFilename.wstring()).c_str(), &this->sql_log, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
		if (result != SQLITE_OK)
		{
			this->sql_log = NULL;
			return FALSE;
		}
	}
	sqlite3_stmt *stmt = NULL;

	int result = sqlite3_prepare_v2(this->sql_log, "CREATE TABLE IF NOT EXISTS config( id integer PRIMARY KEY AUTOINCREMENT, module text NOT NULL, key text NOT NULL, value text NOT NULL ); ", -1, &stmt, NULL);

	if (result == SQLITE_OK) {
		sqlite3_step(stmt);
		sqlite3_finalize(stmt);
		if (id != -1)
		{
			result = sqlite3_prepare_v2(this->sql_log, "DELETE FROM config WHERE module=? AND id=?;", -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_text(stmt, 1, this->sModule.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_int(stmt, 2, id);
				sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}
		else
		{
			result = sqlite3_prepare_v2(this->sql_log, "DELETE FROM config WHERE module=? AND key=?;", -1, &stmt, NULL);
			if (result == SQLITE_OK) {
				sqlite3_bind_text(stmt, 1, this->sModule.c_str(), -1, SQLITE_STATIC);
				sqlite3_bind_text(stmt, 2, sKey.c_str(), -1, SQLITE_STATIC);
				sqlite3_step(stmt);
				sqlite3_finalize(stmt);
			}
		}
	}
	return TRUE;
}

