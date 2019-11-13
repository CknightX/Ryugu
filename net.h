#pragma once
#include <netinet/in.h>
#include <fcntl.h>
#include <string>

namespace ck
{
    class Ipv4Addr final
    {
        public:
        Ipv4Addr(const std::string& host, unsigned short port);
        Ipv4Addr(const sockaddr_in& _addr){}
        Ipv4Addr()=default;
        std::string toString()const{return "";}
        std::string getIp() const {return "";}
        unsigned short getPort() const {return 1;}
        unsigned int getIpInt() const {return 1;}
        bool isIpValid()const {return true;}

        sockaddr_in& getAddr(){return addr;}


        private:
        sockaddr_in addr;

    };

    int setNonBlocking(int fd);

    } // namespace ck