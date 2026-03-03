#pragma once
#include <iostream>
#include "RequestCommand.h"

#define RES_COMMAND_SIZE 2
#define RES_STATUS_SIZE 2 // 0: fail 1: success
// TODO: 한글 주석 warning 조사

class BaseResponse {
public:
	BaseResponse(const short command, bool status, const std::string& message = std::string())
		:_command(command)
		, _status(status)
		, _message(message) {
	}

protected:
	short _command = Request::Command::UNKNOWN;
	bool _status = false;
	std::string _message;

public:
	short getCommand() { return _command;  }
	bool getStatus() { return _status; }
	std::string getMessage() { return _message;  }

	virtual int serialize(char* buffer);
	virtual int deserialize(const char* buffer);
};
