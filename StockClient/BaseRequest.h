#pragma once
#include "BaseResponse.h"
#include "RequestCommand.h"

#define REQ_COMMAND_SIZE 2

class BaseRequest {
public:
	BaseRequest(const short command)
		: _command(command) {}

protected:
	short _command;

public:
	short getCommand() { return _command; }

	virtual int serialize(char* buffer);
	virtual int deserialize(const char* buffer);
};
