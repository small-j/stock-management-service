#include "pch.h"
#include "PrintItemResponse.h"

int PrintItemResponse::serialize(char* buffer) {
	int offset = __super::serialize(buffer);

	memcpy(buffer + offset, _itemList.c_str(), _itemList.size());
	offset += _itemList.size();

	return offset;
}

int PrintItemResponse::deserialize(const char* buffer) {
	int offset = __super::deserialize(buffer);

	int strLength = 0;
	memcpy(&strLength, buffer + offset, sizeof(int));
	offset += sizeof(int);

	_itemList.assign(buffer + offset);

	return offset;
}