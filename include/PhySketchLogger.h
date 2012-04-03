#ifndef Logger_h__
#define Logger_h__
#pragma once

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"

namespace PhySketch
{
	class Logger : public Singleton<Logger>
	{
	public:
		Logger();
		Logger(std::string logfile, bool printToConsole);
		~Logger(void);

		void Write(std::string msg);		
		void WriteWarning(std::string warning);
		void WriteError(std::string error);

		static Logger* getSingletonPtr(void);
		static Logger& getSingleton(void);

	protected:
		void WriteToFile(std::string msg);
		

	protected:
		std::ofstream *_logFile;
		bool _printToConsole;
	};
}

#endif // Logger_h__
