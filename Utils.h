/*
Utils
工具类
*/


#pragma once

#include<string>
#include <string.h>


namespace ryugu
{
    class noncopyable
    {
        protected:
        noncopyable()=default;
        ~noncopyable()=default;

        noncopyable(const noncopyable&) = delete;
        noncopyable& operator=(const noncopyable&) = delete;
    };
	inline void memZero(void* p, size_t n) { memset(p, 0, n); }


    std::string strEvents(uint32_t events);
    std::string strErrno(int _errno);


    

}
