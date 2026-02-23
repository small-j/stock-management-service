#include "AddStockRequest.h"

int AddStockRequest::serialize(char* buffer) {
	int offset = __super::serialize(buffer);

	memcpy(buffer + offset, &_itemId, sizeof(_itemId));
	offset += sizeof(_itemId);

	memcpy(buffer + offset, &_count, sizeof(_count));
	offset += sizeof(_count);

	return offset;
}

int AddStockRequest::deserialize(const char* buffer) {
	int offset = __super::deserialize(buffer);

	memcpy(&_itemId, buffer + offset, sizeof(_itemId));
	offset += sizeof(_itemId);

	memcpy(&_count, buffer + offset, sizeof(_count));
	offset += sizeof(_count);

	return offset;
}