#include "RemoveItemRequest.h"

int RemoveItemRequest::serialize(char* buffer) const {
	int offset = __super::serialize(buffer);

	memcpy(buffer + offset, &_itemId, sizeof(_itemId));
	offset += sizeof(_itemId);

	return offset;
}

int RemoveItemRequest::deserialize(const char* buffer) {
	int offset = __super::deserialize(buffer);

	memcpy(&_itemId, buffer + offset, sizeof(_itemId));
	offset += sizeof(_itemId);

	return offset;
}