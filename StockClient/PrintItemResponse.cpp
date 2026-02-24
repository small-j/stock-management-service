#include "PrintItemResponse.h"

int PrintItemResponse::serialize(char* buffer) {
	int offset = __super::serialize(buffer);

	int strLength = static_cast<int>(_itemList.length());
	memcpy(buffer + offset, &strLength, sizeof(strLength));
	offset += sizeof(strLength);

	memcpy(buffer + offset, _itemList.c_str(), strLength);
	offset += strLength;

	return offset;
}

int PrintItemResponse::deserialize(const char* buffer) {
	int offset = __super::deserialize(buffer);

	int strLength = 0;
	memcpy(&strLength, buffer + offset, sizeof(strLength));
	offset += sizeof(strLength);

	_itemList.assign(buffer + offset, strLength);
	offset += strLength;

	return offset;
}