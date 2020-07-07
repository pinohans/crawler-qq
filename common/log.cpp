#include "stdafx.h"
#include "log.h"


std::filesystem::path troycrawler::log::pLogFilepath;

std::filesystem::path troycrawler::config::pConfigFilename;

std::string GetNow(int level)
{
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	CHAR strTime[MAX_PATH];
	if (level == 1)
	{
		sprintf_s(strTime, WS2U8(L"%04d-%02d-%02d").c_str(),
			stTime.wYear, stTime.wMonth, stTime.wDay);
	}
	else if (level == 2)
	{
		sprintf_s(strTime, WS2U8(L"%04d-%02d-%02d %02d:%02d:%02d").c_str(),
			stTime.wYear, stTime.wMonth, stTime.wDay,
			stTime.wHour, stTime.wMinute, stTime.wSecond);
	}
	return strTime;
}

BOOL troycrawler::config::init()
{
	pConfigFilename = pFilepath / "config.db";

	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;

	result = sqlite3_open_v2(WS2U8(pConfigFilename.wstring()).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
	if (result == SQLITE_OK) {
		result = sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS config( id integer PRIMARY KEY AUTOINCREMENT, key text NOT NULL, value text NOT NULL ); ", -1, &stmt, NULL);

		if (result == SQLITE_OK) {
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			sqlite3_close(db);
			return TRUE;
		}
		sqlite3_close(db);
	}
	return FALSE;
}

BOOL troycrawler::config::add(std::string _sKey, std::string _sValue)
{
	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;
	
	result = sqlite3_open_v2(WS2U8(pConfigFilename.wstring()).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
	if (result == SQLITE_OK) {
		result = sqlite3_prepare_v2(db, "INSERT INTO config(key, value) VALUES(?,?);", -1, &stmt, NULL);

		if (result == SQLITE_OK) {
			sqlite3_bind_text(stmt, 1, _sKey.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_text(stmt, 2, _sValue.c_str(), -1, SQLITE_STATIC);

			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
			
			sqlite3_close(db);
			return TRUE;
		}
		sqlite3_close(db);
	}
	return FALSE;
}

std::map<int, std::string> troycrawler::config::get(std::string _sKey)
{
	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;
	std::map<int, std::string> ret = std::map<int, std::string>();

	result = sqlite3_open_v2(WS2U8(pConfigFilename.wstring()).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
	if (result == SQLITE_OK) {

		result = sqlite3_prepare_v2(db, "SELECT id, value FROM config WHERE key=?", -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_text(stmt, 1, _sKey.c_str(), -1, SQLITE_STATIC);

			while (sqlite3_step(stmt) == SQLITE_ROW) {
				int id = sqlite3_column_int(stmt, 0);
				std::string url = std::string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
				ret[id] = url;
			}
			sqlite3_finalize(stmt);
		}
		sqlite3_close(db);
	}
	return ret;
}

BOOL troycrawler::config::set(int _iId, std::string _sValue)
{
	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;

	result = sqlite3_open_v2(WS2U8(pConfigFilename.wstring()).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);

	if (result == SQLITE_OK) {

		result = sqlite3_prepare_v2(db, "UPDATE config SET value=? WHERE id=?;", -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_text(stmt, 1, _sValue.c_str(), -1, SQLITE_STATIC);
			sqlite3_bind_int(stmt, 2, _iId);

			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			sqlite3_close(db);
			return TRUE;
		}
		sqlite3_close(db);
	}
	return FALSE;
}

BOOL troycrawler::config::del(int _iId)
{
	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;

	result = sqlite3_open_v2(WS2U8(pConfigFilename.wstring()).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);

	if (result == SQLITE_OK) {

		result = sqlite3_prepare_v2(db, "DELETE FROM config WHERE id=?;", -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, _iId);

			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			sqlite3_close(db);
			return TRUE;
		}
		sqlite3_close(db);
	}
	return FALSE;
}

BOOL troycrawler::log::init()
{
	pLogFilepath = pFilepath / sModule;
	std::filesystem::create_directories(pLogFilepath);

	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;

	result = sqlite3_open_v2(WS2U8(getLogFilename().wstring()).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
	if (result == SQLITE_OK) {

		result = sqlite3_prepare_v2(db, "CREATE TABLE IF NOT EXISTS log( id integer PRIMARY KEY AUTOINCREMENT, time text NOT NULL, level text NOT NULL, message text NOT NULL ); ", -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			sqlite3_close(db);
			return TRUE;
		}
		sqlite3_close(db);
	}
	return FALSE;
}

std::filesystem::path troycrawler::log::getLogFilename()
{
	return pLogFilepath / (GetNow(1) + ".db");
}

BOOL troycrawler::log::error(std::string _sMessage)
{
	return log(LOGERROR, _sMessage);
}

BOOL troycrawler::log::warn(std::string _sMessage)
{
	return log(LOGWARN, _sMessage);
}

BOOL troycrawler::log::info(std::string _sMessage)
{
	return log(LOGINFO, _sMessage);
}

BOOL troycrawler::log::debug(std::string _sMessage)
{
	return log(LOGDEBUG, _sMessage);
}

BOOL troycrawler::log::log(troycrawler::log::loglevel _lLoglevel, std::string _sMessage)
{
	troycrawler::log::init();
	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;

	result = sqlite3_open_v2(WS2U8(getLogFilename().wstring()).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
	if (result == SQLITE_OK) {

		result = sqlite3_prepare_v2(db, "INSERT INTO log(time, level, message) VALUES(?,?,?);", -1, &stmt, NULL);
		if (result == SQLITE_OK) {
			sqlite3_bind_text(stmt, 1, GetNow(2).c_str(), -1, SQLITE_STATIC);
			switch (_lLoglevel)
			{
			case LOGERROR:
				sqlite3_bind_text(stmt, 2, "error", -1, SQLITE_STATIC);
				break;
			case LOGWARN:
				sqlite3_bind_text(stmt, 2, "warn", -1, SQLITE_STATIC);
				break;
			case LOGINFO:
				sqlite3_bind_text(stmt, 2, "info", -1, SQLITE_STATIC);
				break;
			case LOGDEBUG:
				sqlite3_bind_text(stmt, 2, "debug", -1, SQLITE_STATIC);
				break;
			default:
				break;
			}
			sqlite3_bind_text(stmt, 3, _sMessage.c_str(), -1, SQLITE_STATIC);

			sqlite3_step(stmt);
			sqlite3_finalize(stmt);

			sqlite3_close(db);
			return TRUE;
		}
		sqlite3_close(db);
	}
	return FALSE;
}

std::vector<troycrawler::log::Message> troycrawler::log::get(std::string _sModule, std::string _sFilename)
{

	std::filesystem::path pLogFilename = pFilepath / _sModule / _sFilename;
	std::vector<troycrawler::log::Message> ret = std::vector<troycrawler::log::Message>();
	if (!std::filesystem::exists(pLogFilename))	return ret;

	sqlite3 * db;
	sqlite3_stmt * stmt;
	int result;
	int size = 0;

	result = sqlite3_open_v2(WS2U8(pLogFilename.wstring()).c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX | SQLITE_OPEN_SHAREDCACHE, NULL);
	if (result == SQLITE_OK) {
		result = sqlite3_prepare_v2(db, "SELECT count(*) FROM log;", -1, &stmt, NULL);

		if (result == SQLITE_OK) {

			if (sqlite3_step(stmt) == SQLITE_ROW) {
				size = sqlite3_column_int(stmt, 0);
			}
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
		size = min(size, 2000);
		result = sqlite3_prepare_v2(db, "SELECT time, level, message FROM log ORDER BY id DESC LIMIT 0,?;", -1, &stmt, NULL);

		if (result == SQLITE_OK) {
			sqlite3_bind_int(stmt, 1, size);
			while (sqlite3_step(stmt) == SQLITE_ROW) {
				ret.push_back(Message(sqlite3_column_text(stmt, 0), sqlite3_column_text(stmt, 1), sqlite3_column_text(stmt, 2)));
			}
			sqlite3_step(stmt);
			sqlite3_finalize(stmt);
		}
		sqlite3_close(db);
	}
	return ret;
}
