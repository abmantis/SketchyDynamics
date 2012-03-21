#include "StdAfx.h"
#include "Logger.h"

namespace PhySketch
{

template<> Logger* Singleton<Logger>::ms_Singleton = 0;
Logger::Logger(void)
{
}

Logger::~Logger(void)
{
}

Logger* Logger::getSingletonPtr(void)
{
	assert(ms_Singleton != NULL);
	return ms_Singleton;
}

Logger& Logger::getSingleton(void)
{
	assert(ms_Singleton != NULL);
	return *ms_Singleton;
}

}