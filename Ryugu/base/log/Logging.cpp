#include "Logging.h"

namespace ryugu
{
	Logger::Logger(std::string file, int line, LogLevel logLevel)
		:logLevel_(logLevel)
	{
		if (checkPrintOrNot())
		{
			setColor();
			beginFormat(file, line);
		}
	}
	Logger::~Logger()
	{
		if (checkPrintOrNot())
		{
			endFormat();
			safePrint(ss_.str());
		}
	}
	void Logger::beginFormat(std::string file, int line)
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
	void Logger::endFormat()
	{
		ss_ << "\033[0m\n";
	}
	void Logger::setColor()
	{
		switch (logLevel_)
		{
		case LogLevel::DEBUG:  // white
			ss_ << "\033[37m";
			break;
		case LogLevel::INFO:   // green
			ss_ << "\033[32m";
			break;
		case LogLevel::WARN:   // yello
			ss_ << "\033[33m";
			break;
		case LogLevel::ERROR:  // red
			ss_ << "\033[31m";
			break;
		}
	}
	Logger::LogLevel Logger::g_logLevel_ = LogLevel::INFO;  // default level:info
	std::mutex Logger::mtx_;

}
