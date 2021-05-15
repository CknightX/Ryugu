#include <sys/timerfd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <fstream>
#include "Ryugu/base/Utils.h"
#include "Ryugu/base/Debug.h"

namespace ryugu
{
	namespace base
	{
		std::string strEvents(uint32_t events)
		{
			//TODO
			return "";
		}
		std::string strErrno(int _errno)
		{
			//TODO
			return "";
		}
		std::string readFile(const std::string& path)
		{
			std::ifstream ifs(path);
			std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			return content;
		}
		bool fileExist(const std::string& path)
		{
			struct stat sbuf;
			if (stat(path.c_str(), &sbuf) < 0)
			{
				return false;
			}
			return true;
		}

	}
}