#pragma once

class BaseRequest {
public:
	BaseRequest(const short command)
		:_command(command) {}

private:
	short _command;
};
