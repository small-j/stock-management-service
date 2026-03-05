#include "pch.h"
#include "BaseRequest.h"

int BaseRequest::serialize(char* buffer) const {
	const short command = getCommand();
	int offset = 0;

	memcpy(buffer + offset, &command, REQ_COMMAND_SIZE);
	offset += REQ_COMMAND_SIZE;

	return offset;
}

int BaseRequest::deserialize(const char* buffer) {
	int offset = 0;

	memcpy(&_command, buffer + offset, REQ_COMMAND_SIZE);
	offset += REQ_COMMAND_SIZE;

	return offset;
}