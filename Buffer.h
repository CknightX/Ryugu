/**
 * Buffer
 */
#pragma once
#include <cstddef>
#include <vector>
#include <string>

namespace ryugu
{
    class Buffer
    {
        public:

        Buffer();
        ~Buffer();

        void writeIn(const char* str,size_t len);
        void writeIn(const std::string& str);
        void readOut(char* _buf,size_t len);
        // 以string类型读出
        std::string readOutAsString(size_t len);

        void consume(size_t len){bg+=len;}


        void clear();

        size_t size() const{return ed-bg;}

        // 获取字符串指针
        const char* getData()const {return buf.data(); }
        bool empty() const {return size()==0;}
        

        private:
        // [bg,ed) 所指示区域为有效数据
        size_t bg,ed;
        std::vector<char> buf;

    };

}