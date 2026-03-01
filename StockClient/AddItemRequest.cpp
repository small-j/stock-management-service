#include "AddItemRequest.h"

int AddItemRequest::serialize(char* buffer) const {
	int offset = __super::serialize(buffer);

	int nameLen = static_cast<int>(_name.length());
	memcpy(buffer + offset, &nameLen, sizeof(nameLen)); // name 길이 먼저 기록
	offset += sizeof(nameLen);

	memcpy(buffer + offset, _name.c_str(), nameLen); // name 기록
	offset += nameLen;

	memcpy(buffer + offset, &_itemType, sizeof(_itemType)); // itemType 기록
	offset += sizeof(_itemType);

	return offset;
}

int AddItemRequest::deserialize(const char* buffer) {
	int offset = __super::deserialize(buffer);

	int nameLen = 0;
	memcpy(&nameLen, buffer + offset, sizeof(nameLen));
	offset += sizeof(nameLen);

	_name.assign(buffer + offset, nameLen);
	offset += nameLen;

	memcpy(&_itemType, buffer + offset, sizeof(_itemType));
	offset += sizeof(_itemType);

	return offset;
}