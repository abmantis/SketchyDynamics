#ifndef Logger_h__
#define Logger_h__
#pragma once

#include "Singleton.h"

namespace PhySketch
{
	class Logger : public Singleton<Logger>
	{
	public:
		Logger(void);
		~Logger(void);

		static Logger* getSingletonPtr(void);
		static Logger& getSingleton(void);
	};
}

#endif // Logger_h__
