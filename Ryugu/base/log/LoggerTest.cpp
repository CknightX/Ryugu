#include "Logging.h"

int main()
{
	ryugu::Logger(__FILE__,__LINE__,ryugu::Logger::LogLevel::WARN) << "abc" << 123;
}