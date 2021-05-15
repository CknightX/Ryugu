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
			DEBUG=1,
			INFO,
			WARN,
			ERROR,
		};

		Logger(std::string file, int line,LogLevel logLevel)
			:logLevel_(logLevel)
		{
			if (checkPrintOrNot())
			{
				setColor();
				beginFormat(file, line);
			}
		}
		~Logger()
		{
			if (checkPrintOrNot())
			{
				endFormat();
				safePrint(ss_.str());
			}
		}
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
		void beginFormat(std::string file, int line)
		{
			ss_ << ryugu::base::Timestamp::getNow().toPrettyString() << " - ";
			ss_ << file << "[line:" << line << "]" << " - ";
			switch (logLevel_)
			{
			case LogLevel::DEBUG:
				ss_ << "DEBUG";
				break;
			case LogLevel::INFO:
				ss_ << "INFO";
				break;
			case LogLevel::WARN:
				ss_ << "WARNING";
				break;
			case LogLevel::ERROR:
				ss_ << "ERROR";
				break;
			}
			ss_ << " ";
		}
		void endFormat()
		{
			ss_ << "\033[0m\n";
		}
		void setColor()
		{
			switch (logLevel_)
			{
			case LogLevel::DEBUG:
				ss_ << "\033[37m";
				break;
			case LogLevel::INFO:
				ss_ << "\033[34m";
				break;
			case LogLevel::WARN:
				ss_ << "\033[33m";
				break;
			case LogLevel::ERROR:
				ss_ << "\033[31m";
				break;
			}
		}

	};
	Logger::LogLevel Logger::g_logLevel_ = LogLevel::INFO;  // default level:info
	std::mutex Logger::mtx_;
}


