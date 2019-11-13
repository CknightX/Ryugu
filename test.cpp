#include<vector>
#include<iostream>
#include <string>
#include <typeinfo>
#include "buffer.h"
#include "debug.h"
using namespace std;
using ck::Buffer;

int main()
{
    Buffer buf;
    cout<<buf.size()<<endl;
    for (auto i=0;i<100;++i)
        buf.writeIn("1");
    auto tmp=buf.readOutAsString(50);

    for (auto i=0;i<78;++i)
        buf.writeIn("1");

    LOG("size=%d",buf.size());
    LOG("cap=%d",buf.buf.capacity());


}
