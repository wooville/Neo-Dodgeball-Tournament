#include "stdafx.h"
#include "Logger.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <windows.h>

std::vector<LogEntry> Logger::messages;

std::string ThisDateTimeToString() {
	std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	std::string output(30, '\0');
	std::strftime(&output[0], output.size(), "%d/%b/%Y %H:%M:%S", std::localtime(&now));
	return output;
}

void Logger::Log(const std::string& message) {
	LogEntry logEntry;
	logEntry.type = LOG_INFO;
	logEntry.message = "LOG | " + ThisDateTimeToString() + " - " + message;
	std::cout <<  logEntry.message << std::endl;

	messages.push_back(logEntry);
}

void Logger::Err(const std::string& message) {
	LogEntry logEntry;
	logEntry.type = LOG_ERROR;
	logEntry.message = "ERR | " + ThisDateTimeToString() + " - " + message;
	std::cout << logEntry.message << std::endl;

	messages.push_back(logEntry);
}