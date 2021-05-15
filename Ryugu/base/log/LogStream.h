#pragma once
#include <iostream>
#include <string>
namespace ryugu
{
	class LogStream
	{
	public:
		LogStream(int logLevel):logLevel_(logLevel){}
		template <typename T>
		LogStream& operator<<(T content);
	private:
		int logLevel_;
		std::string fmtContent;
	};

	template<class T>
	LogStream& LogStream::operator<<(T content)
	{
		switch (logLevel_)
		{
		case 0:
			fmtContent.append("[TRACE]");
			break;
		case 1:
			fmtContent.append("\033[37m[DEBUG]");
			break;
		case 2:
			fmtContent.append("\033[34m[INFO]");
			break;
		case 3:
			fmtContent.append("\033[33m[WARN]");
			break;
		case 4:
			fmtContent.append("\033[31m[ERROR]");
			break;
		case 5:
			fmtContent.append("[FATAL]");
			break;
		}
		fmtContent.append("\033[0m");
		fmtContent.push_back('\n');
		std::cout << fmtContent << std::flush;
	}
}


