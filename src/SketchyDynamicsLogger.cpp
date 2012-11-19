
#include "SketchyDynamicsLogger.h"
#include "SketchyDynamicsUtils.h"

#ifndef SKETCHYDYNAMICS_LOGWARNINGMSG
	#define SKETCHYDYNAMICS_LOGWARNINGMSG "[WARNING] " 
#endif
#ifndef SKETCHYDYNAMICS_LOGERRORMSG
	#define SKETCHYDYNAMICS_LOGERRORMSG "[ERROR] " 
#endif


namespace SketchyDynamics
{

	void __sketchydynamics_assert__(const char * file, unsigned int line, const char * expr, bool dump)
	{
		Logger *l = Logger::getSingletonPtr();
		if(l)
		{
			l->writeError(std::string(file) + ":" + toString((ulong)line), std::string("Assertion failed: ") + expr);
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
	SKETCHYDYNAMICS_ASSERT(ms_Singleton != NULL);
	return ms_Singleton;
}

Logger& Logger::getSingleton(void)
{
	SKETCHYDYNAMICS_ASSERT(ms_Singleton != NULL);
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
			<< Tm->tm_sec<< "]" << msg << "\n";
		_logFile->flush();
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
	writeToFile(SKETCHYDYNAMICS_LOGWARNINGMSG + warning);

	if(_printToConsole)
	{
		std::cout << SKETCHYDYNAMICS_LOGWARNINGMSG << warning << std::endl;
	}
}

void Logger::writeWarning( std::string location, std::string warning )
{
	writeWarning("{" + location + "} " + warning);
}

void Logger::writeError( std::string error )
{
	writeToFile(SKETCHYDYNAMICS_LOGERRORMSG + error);
	
	if(_printToConsole)
	{
		std::cerr << SKETCHYDYNAMICS_LOGERRORMSG << error << std::endl;
	}	
}

void Logger::writeError( std::string location, std::string error )
{
	writeError("{" + location + "} " + error);
}

}