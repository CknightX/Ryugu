/*
EventHandler
处理Channel与Poller之间的交互，Channel与Poller之间不直接交互
*/
#pragma once
#include "poller.h"
#include "debug.h"

namespace ck
{

class Channel;


// 事件处理器,将事件分发到poller中
class EventHandler 
{
    public:
    EventHandler():isStop(false),poller(PollerFactory::getPoller()){}
    ~EventHandler(){}


    void loopOnce(int waitMs);
    void loop();
    void stop(){isStop=true;}


    public:
    Poller* poller;

    private:
    bool isStop;



    

};
}