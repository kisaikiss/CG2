#pragma once
#include <string>

class Logger {
public:
	static std::string logFilePath;

	static void LogInit();

	static void Log(const std::string& message);
private:
};

