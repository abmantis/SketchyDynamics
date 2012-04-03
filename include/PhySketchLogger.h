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
		virtual ~Logger(void);

		virtual void writeMessage(std::string msg);		
		virtual void writeWarning(std::string warning);
		virtual void writeError(std::string error);

		static Logger* getSingletonPtr(void);
		static Logger& getSingleton(void);

	protected:
		virtual void writeToFile(std::string msg);
		

	protected:
		std::ofstream *_logFile;
		bool _printToConsole;
	};
}

#endif // Logger_h__
