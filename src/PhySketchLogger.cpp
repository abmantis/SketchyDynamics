
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
	_logFile->open(logfile, std::ios_base::out);
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

void Logger::writeToFile( std::string msg )
{
	if(_logFile != NULL)
	{
		time_t ltime;
		struct tm *Tm;
		ltime=time(NULL);
		Tm=localtime(&ltime);
		
		// print message with added timestamp
		(*_logFile) << "[" << Tm->tm_year+1900 << "-" << Tm->tm_mon+1 << "-" 
			<< Tm->tm_mday << " " << Tm->tm_hour<< ":" << Tm->tm_min << ":"
			<< Tm->tm_sec<< "]" << msg;
	}
}

void Logger::writeMessage( std::string msg )
{
	writeToFile(msg);

	if(_printToConsole)
	{
		std::cout << msg << std::endl;
	}
}

void Logger::writeWarning( std::string warning )
{
	writeToFile(PHYSKETCH_LOG_WARNING + warning);

	if(_printToConsole)
	{
		std::cout << PHYSKETCH_LOG_WARNING << warning << std::endl;
	}
}

void Logger::writeWarning( std::string location, std::string warning )
{
	writeWarning("{" + location + "} " + warning);
}

void Logger::writeError( std::string error )
{
	writeToFile(PHYSKETCH_LOG_ERROR + error);
	
	if(_printToConsole)
	{
		std::cerr << PHYSKETCH_LOG_ERROR << error << std::endl;
	}	
}

void Logger::writeError( std::string location, std::string error )
{
	writeError("{" + location + "} " + error);
}

}