#ifndef Logger_h__
#define Logger_h__
#pragma once

#include "SketchyDynamicsDefinitions.h"
#include "SketchyDynamicsSingleton.h"

namespace SketchyDynamics
{
#define SKETCHYDYNAMICS_LOG_MESSAGE(msg)	Logger::getSingletonPtr()->writeMessage( STRINGIZE(__FILE__) ":" STRINGIZE(__LINE__), msg)
#define SKETCHYDYNAMICS_LOG_WARNING(msg)	Logger::getSingletonPtr()->writeWarning( STRINGIZE(__FILE__) ":" STRINGIZE(__LINE__), msg)
#define SKETCHYDYNAMICS_LOG_ERROR(msg)	Logger::getSingletonPtr()->writeError( STRINGIZE(__FILE__) ":" STRINGIZE(__LINE__), msg)

	class Logger : public Singleton<Logger>
	{
	public:
		Logger();
		Logger(std::string logfile, bool printToConsole);
		virtual ~Logger(void);

		/// <summary> Writes a message to the log. </summary>
		/// <param name="msg"> The message. </param>
		virtual void writeMessage(std::string msg);		

		/// <summary> Writes a message to the log. </summary>
		/// <param name="location"> The location of the error (class/function). </param>
		/// <param name="msg"> The message. </param>
		virtual void writeMessage(std::string location, std::string msg);		

		/// <summary> Writes a warning to the log. </summary>
		/// <param name="warning"> The warning. </param>
		virtual void writeWarning(std::string warning);

		/// <summary> Writes a warningto the log. </summary>
		/// <param name="location"> The location of the error (class/function). </param>
		/// <param name="warning"> The warning. </param>
		virtual void writeWarning(std::string location, std::string warning);
		
		/// <summary> Writes an error to the log. </summary>
		/// <param name="error"> The error. </param>
		virtual void writeError(std::string error);

		/// <summary> Writes an error to the log. </summary>
		/// <param name="location"> The location of the error (class/function). </param>
		/// <param name="error"> The error. </param>
		virtual void writeError(std::string location, std::string error);

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
