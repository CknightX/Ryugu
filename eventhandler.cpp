#include "eventhandler.h"
namespace ck{

void EventHandler::loopOnce(int waitMs)
{
    poller->loopOnce(waitMs);
}
void EventHandler::loop()
{
    while(!isStop)
    {
        loopOnce(500);
    }
}
}