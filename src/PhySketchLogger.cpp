
#include "PhySketchLogger.h"
#include "PhySketchUtils.h"

#ifndef PHYSKETCH_LOGWARNINGMSG
	#define PHYSKETCH_LOGWARNINGMSG "[WARNING] " 
#endif
#ifndef PHYSKETCH_LOGERRORMSG
	#define PHYSKETCH_LOGERRORMSG "[ERROR] " 
#endif


namespace PhySketch
{

	void __physketch_assert__(const char * file, unsigned int line, const char * expr, bool dump)
	{
		Logger *l = Logger::getSingletonPtr();
		if(l)
		{
			l->writeError(std::string(file) + ":" + toString(line), std::string("Assertion failed: ") + expr);
		}
		else
		{
			std::fprintf(stderr, "Assertion failed at line %u in file %s: %s\n", line, file, expr);		
		}
		std::fflush(NULL);
		if (dump) std::abort();
	}

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
	PHYSKETCH_ASSERT(ms_Singleton != NULL);
	return ms_Singleton;
}

Logger& Logger::getSingleton(void)
{
	PHYSKETCH_ASSERT(ms_Singleton != NULL);
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

void Logger::writeMessage( std::string location, std::string msg )
{
	writeMessage("{" + location + "} " + msg);
}

void Logger::writeWarning( std::string warning )
{
	writeToFile(PHYSKETCH_LOGWARNINGMSG + warning);

	if(_printToConsole)
	{
		std::cout << PHYSKETCH_LOGWARNINGMSG << warning << std::endl;
	}
}

void Logger::writeWarning( std::string location, std::string warning )
{
	writeWarning("{" + location + "} " + warning);
}

void Logger::writeError( std::string error )
{
	writeToFile(PHYSKETCH_LOGERRORMSG + error);
	
	if(_printToConsole)
	{
		std::cerr << PHYSKETCH_LOGERRORMSG << error << std::endl;
	}	
}

void Logger::writeError( std::string location, std::string error )
{
	writeError("{" + location + "} " + error);
}

}