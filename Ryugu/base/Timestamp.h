#pragma once
#include <cstdint>
#include <string>

namespace ryugu
{
	namespace base
	{

		class Timestamp
		{
		public:
			static const int cstMicroSecondsPerSecond = 1000 * 1000;

			Timestamp()
				: microSecondsSinceLastPoll_(0)
			{
			}
			explicit Timestamp(int64_t seconds)
				: microSecondsSinceLastPoll_(seconds)
			{
			}

			static Timestamp getNow();

			int64_t getTimestampByMicroSeconds() const { return microSecondsSinceLastPoll_; }
			time_t getTimestampBySeconds() const { return microSecondsSinceLastPoll_ / cstMicroSecondsPerSecond; }
			bool isValid() const { return microSecondsSinceLastPoll_ > 0; }
			Timestamp addTime(const double seconds)
			{
				int64_t delta = static_cast<int64_t>(seconds * cstMicroSecondsPerSecond);
				microSecondsSinceLastPoll_ += delta;
				return *this;
			}

			std::string toString() const;
			std::string toPrettyString(bool showMicroseconds=true) const;

			static Timestamp invalid()
			{
				return Timestamp();
			}



			bool operator< (const Timestamp& t) const { return microSecondsSinceLastPoll_ < t.microSecondsSinceLastPoll_; }
			bool operator== (const Timestamp& t) const { return microSecondsSinceLastPoll_ == t.microSecondsSinceLastPoll_; }
			bool operator> (const Timestamp& t) const { return microSecondsSinceLastPoll_ > t.microSecondsSinceLastPoll_; }
			bool operator<= (const Timestamp& t) const { return microSecondsSinceLastPoll_ <= t.microSecondsSinceLastPoll_; }
			bool operator>= (const Timestamp& t) const { return microSecondsSinceLastPoll_ >= t.microSecondsSinceLastPoll_; }

		private:
			int64_t microSecondsSinceLastPoll_;
		};
	}
} // namespace ryugu