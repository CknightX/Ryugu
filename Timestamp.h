#pragma once
#include <cstdint>
#include <string>

namespace ck
{


class Timestamp
{
public:
    static const int cstMicroSecondsPerSecond=1000*1000;

    Timestamp()
        : microSecondsSinceLastPoll(0)
    {
    }
    explicit Timestamp(int64_t seconds)
        : microSecondsSinceLastPoll(seconds)
    {
    }

    static Timestamp getNow();

    int64_t getTimestampByMicroSeconds() const {return microSecondsSinceLastPoll;}
    time_t getTimestampBySeconds() const {return microSecondsSinceLastPoll/cstMicroSecondsPerSecond;}
    bool isValid() const { return microSecondsSinceLastPoll > 0; }
    Timestamp addTime(const double seconds)
    {
        int64_t delta=static_cast<int64_t>(seconds*cstMicroSecondsPerSecond);
        microSecondsSinceLastPoll+=delta;
        return *this;
    }

    std::string toString() const;

    static Timestamp invalid()
    {
        return Timestamp();
    }



    bool operator< (const Timestamp& t) const{return microSecondsSinceLastPoll<t.microSecondsSinceLastPoll;}
    bool operator== (const Timestamp& t) const{return microSecondsSinceLastPoll==t.microSecondsSinceLastPoll;}
    bool operator> (const Timestamp& t) const{return microSecondsSinceLastPoll>t.microSecondsSinceLastPoll;}
    bool operator<= (const Timestamp& t) const{return microSecondsSinceLastPoll<=t.microSecondsSinceLastPoll;}
    bool operator>= (const Timestamp& t) const{return microSecondsSinceLastPoll>=t.microSecondsSinceLastPoll;}

private:
    int64_t microSecondsSinceLastPoll;
};
} // namespace ck