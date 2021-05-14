#pragma once
#include <netinet/in.h>
#include <fcntl.h>
#include <string>

namespace ryugu
{
    namespace net
    {
        class Ipv4Addr final
        {
            public:
                Ipv4Addr()=default;
                Ipv4Addr(const std::string& host, uint16_t port);
                Ipv4Addr(const sockaddr_in& _addr):addr(_addr){}
                std::string toString()const{return "";}
                std::string getIp() const {return "";}
                uint16_t getPort() const {return 1;}
                unsigned int getIpInt() const {return 1;}
                bool isIpValid()const {return true;}

                sockaddr_in& getAddr() {return addr;}
            private:
                sockaddr_in addr;
        };
        int setNonBlocking(int fd);
    }
} // namespace ryugu
