#pragma once
#include <iostream>

class BaseResponse {
public:
	BaseResponse(const short command, bool status, std::string& message)
		:_command(command)
		,_status(status)
		,_message(message) {
	}

private:
	short _command;
	bool _status;
	std::string message;
};
