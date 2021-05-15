#pragma once
#include <iostream>
#include <mutex>
#include <sstream>
#include "Ryugu/base/Timestamp.h"


namespace ryugu
{
	class Logger
	{
	public:
		enum class LogLevel
		{
			DEBUG,
			INFO,
			WARN,
			ERROR,
		};

		Logger(std::string file, int line, LogLevel logLevel);
		~Logger();
		template<typename T>
		Logger& operator<<(const T& obj)
		{
			if (checkPrintOrNot())
				ss_ << obj;
			return *this;
		}
		bool checkPrintOrNot() { return logLevel_ >= g_logLevel_; }
		static LogLevel getGLogLevel() { return g_logLevel_; }
		static void setGLogLevel(LogLevel logLevel) { g_logLevel_ = logLevel; }
	private:
		std::stringstream ss_;
		static LogLevel g_logLevel_;  // default level:info
		LogLevel logLevel_;
		static std::mutex mtx_;
		static void safePrint(const std::string& content)
		{
			std::lock_guard<std::mutex> lk(mtx_);
			std::cout << content << std::flush;
		}
		void beginFormat(std::string file, int line);
		void endFormat();
		void setColor();
	};

	#define LOG_DEBUG Logger(__FILE__,__LINE__,ryugu::Logger::LogLevel::DEBUG)
	#define LOG_INFO Logger(__FILE__,__LINE__,ryugu::Logger::LogLevel::INFO)
	#define LOG_WARN Logger(__FILE__,__LINE__,ryugu::Logger::LogLevel::WARN)
	#define LOG_ERROR Logger(__FILE__,__LINE__,ryugu::Logger::LogLevel::ERROR)


}


