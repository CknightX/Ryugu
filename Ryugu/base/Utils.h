#pragma once

#include<string>
#include <string.h>

namespace ryugu
{
	inline void memZero(void* p, size_t n) { memset(p, 0, n); }
    std::string strEvents(uint32_t events);
    std::string strErrno(int _errno);
}
