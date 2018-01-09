#include "sgflog.h"

#include <iostream>

#pragma warning(disable:4996)

namespace SGF
{
Logger::Logger()
{
	BOOL bRes = AllocConsole();
	if (!bRes) {
		return;
	}

	freopen("CONOUT$", "w", stdout);

	try
	{
		std::vector<spdlog::sink_ptr> sinks;
		sinks.push_back(std::make_shared<spdlog::sinks::wincolor_stdout_sink_st>());
		sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_st>("file_logger.txt", 1024 * 1024 * 5, 3));
		_logger = std::make_shared<spdlog::logger>("logger", begin(sinks), end(sinks));
		spdlog::register_logger(_logger);
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::cout << "Log initialization failed: " << ex.what() << std::endl;
		fclose(stdout);
		FreeConsole();
		return;
	}
}

Logger::~Logger()
{
	fclose(stdout);
	FreeConsole();
}
}