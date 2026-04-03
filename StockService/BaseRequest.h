#pragma once
#include "common.h"

#define REQ_COMMAND_SIZE 2

class BaseRequest {
public:
	BaseRequest(const short command) : _command(command) {}
	virtual ~BaseRequest() = default;

protected:
	short _command;

public:
	short getCommand() const { return _command; }

	virtual int serialize(char* buffer) const;
	virtual int deserialize(const char* buffer);
};
