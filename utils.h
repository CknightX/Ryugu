/*
Utils
工具类
*/


#pragma once

#include<string>


namespace ck
{
    class noncopyable
    {
        protected:
        noncopyable()=default;
        ~noncopyable()=default;

        noncopyable(const noncopyable&)=delete;
        noncopyable& operator=(const noncopyable&) = delete;


    };


    std::string strEvents(uint32_t events);
    std::string strErrno(int _errno);


    

}
