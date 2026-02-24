#include "pch.h"
#include "BaseResponse.h"

int BaseResponse::serialize(char* buffer) {
	int offset = 0;

	memcpy(buffer + offset, &_status, RES_STATUS_SIZE);
	offset += RES_STATUS_SIZE;

	int messageSize = static_cast<int>(_message.length());
	memcpy(buffer + offset, &messageSize, sizeof(messageSize));
	offset += sizeof(messageSize);

	memcpy(buffer + offset, _message.c_str(), messageSize);
	offset += messageSize;

	return offset;
}

int BaseResponse::deserialize(const char* buffer) {
	int offset = 0;

	memcpy(&_status, buffer + offset, RES_STATUS_SIZE);
	offset += RES_STATUS_SIZE;

	int messageSize = 0; // TODO: 멤버변수로 만들어야 할지 의견 여쭤보기
	memcpy(&messageSize, buffer + offset, sizeof(messageSize));
	offset += sizeof(messageSize);

	_message.assign(buffer + offset, messageSize);
	offset += messageSize;

	return offset;
}