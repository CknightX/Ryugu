#pragma once

#define DEBUG


#include <cstdio>
#include <iostream>
#include <inttypes.h>

#ifdef DEBUG

#define _LOG(type,msg,...) printf("[%s] FILE:%s LINE:%d FUNC:%s\n{" msg "}\n",type,__FILE__,__LINE__,__PRETTY_FUNCTION__,\
							##__VA_ARGS__)
#define LOG(msg,args...) _LOG("LOG",msg,##args)
#define LOG_ERROR(msg,args...) _LOG("ERROR",msg,##args)

#define MARK _LOG("----------MARK","");


#define private public
#define protected public


#else

#define LOG(...)
#define _LOG(...)
#define ERROR_LOG(...)



#endif

