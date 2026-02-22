#include "BaseResponse.h"

int BaseResponse::serialize(char* buffer) {
	int offset = 0;

	memcpy(buffer, &_status, RES_STATUS_SIZE);
	offset += RES_STATUS_SIZE;

	memcpy(buffer + offset, _message.c_str(), RES_MESSAGE_SIZE);
	offset += RES_MESSAGE_SIZE;

	return offset;
}

int BaseResponse::deserialize(const char* buffer) {
	int offset = 0;

	memcpy(&_status, buffer + offset, RES_STATUS_SIZE);
	offset += RES_STATUS_SIZE;

	_message.assign(buffer + offset, RES_MESSAGE_SIZE);
	offset += RES_MESSAGE_SIZE;

	return offset;
}