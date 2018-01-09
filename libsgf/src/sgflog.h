#pragma once

#include "spdlog/spdlog.h"

namespace SGF
{
	class Logger
	{
	private:
		Logger();
		~Logger();
		Logger(const Logger &) {};
		Logger &operator = (const Logger &) {};

	private:
		std::shared_ptr<spdlog::logger> _logger;

	public:
		static spdlog::logger* getInstance()
		{
			static Logger instance;
			return instance._logger.get();
		}
	};
}



