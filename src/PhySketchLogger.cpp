
#include "PhySketchLogger.h"


#ifndef PHYSKETCH_LOG_WARNING
	#define PHYSKETCH_LOG_WARNING "[WARNING] " 
#endif
#ifndef PHYSKETCH_LOG_ERROR
	#define PHYSKETCH_LOG_ERROR "[ERROR] " 
#endif

namespace PhySketch
{

template<> Logger* Singleton<Logger>::ms_Singleton = 0;


Logger::Logger()
{
	_printToConsole = true;
	_logFile = NULL;
}

Logger::Logger( std::string logfile, bool printToConsole )
{
	_logFile = new std::ofstream;
	_logFile->open(logfile);
	_printToConsole = printToConsole;
}

Logger::~Logger(void)
{
	if(_logFile != NULL)
	{
		_logFile->close();
		delete _logFile;
		_logFile = NULL;
	}
	
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

void Logger::WriteToFile( std::string msg )
{
	if(_logFile != NULL)
	{
		time_t ltime;
		struct tm *Tm;
		ltime=time(NULL);
		Tm=localtime(&ltime);
		
		// print message with added timestamp
		(*_logFile) << "[" << Tm->tm_year+1900 << " " << Tm->tm_mon+1 << " " 
			<< Tm->tm_mday << " " << Tm->tm_hour<< ":" << Tm->tm_min << ":"
			<< Tm->tm_sec<< "]" << msg;
	}
}

void Logger::Write( std::string msg )
{
	WriteToFile(msg);

	if(_printToConsole)
	{
		std::cout << msg << std::endl;
	}
}

void Logger::WriteWarning( std::string warning )
{
	WriteToFile(PHYSKETCH_LOG_WARNING + warning);

	if(_printToConsole)
	{
		std::cout << PHYSKETCH_LOG_WARNING << warning << std::endl;
	}
}

void Logger::WriteError( std::string error )
{
	WriteToFile(PHYSKETCH_LOG_ERROR + error);
	
	if(_printToConsole)
	{
		std::cerr << PHYSKETCH_LOG_ERROR << error << std::endl;
	}	
}

}