#pragma once

#include "common.h"

class LoggerService {
public:
	static void debug(std::string msg);
	static void warning(std::string msg);
	static void error(std::string msg);
};
