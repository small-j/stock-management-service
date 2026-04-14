#include "pch.h"
#include "LoggerService.h"

void LoggerService::debug(std::string msg) {
	std::cout << "[DEBUG] " << msg << std::endl;
}

void LoggerService::warning(std::string msg) {
	std::cout << "[WARNING] " <<  msg << std::endl;
}

void LoggerService::error(std::string msg) {
	std::cout << "[ERROR] " << msg << std::endl;
}
