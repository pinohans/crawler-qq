#pragma once
namespace troycrawler
{
	namespace log
	{
		struct Message
		{
			std::string sTime;
			std::string sLevel;
			std::string sMessage;

			Message() {}
			Message(
				const unsigned char* _sTime,
				const unsigned char* _sLevel,
				const unsigned char* _sMessage) {
				this->sTime = std::string(reinterpret_cast<const char*>(_sTime));
				this->sLevel = std::string(reinterpret_cast<const char*>(_sLevel));
				this->sMessage = std::string(reinterpret_cast<const char*>(_sMessage));
			}
		};
		enum loglevel { LOGERROR, LOGWARN, LOGINFO, LOGDEBUG };

		extern std::filesystem::path pLogFilepath;

		BOOL init();
		
		std::filesystem::path getLogFilename();

		BOOL error(std::string _sMessage);
		BOOL warn(std::string _sMessage);
		BOOL info(std::string _sMessage);
		BOOL debug(std::string _sMessage);
		BOOL log(troycrawler::log::loglevel _lLoglevel, std::string _sMessage);
		std::vector<troycrawler::log::Message> get(std::string _sModule, std::string _sFilename);
	};
	
	namespace config
	{
		extern std::filesystem::path pConfigFilename;

		BOOL init();

		BOOL add(std::string _sKey, std::string _sValue);
		std::map <int,std::string> get(std::string _sKey);
		BOOL set(int _iId, std::string _sValue);
		BOOL del(int _iId);
	};
};