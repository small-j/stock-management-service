#pragma once
#include "common.h"

class RemoveItemDto {
public:
	RemoveItemDto(unsigned int itemId) : _itemId(itemId) {}

private:
	unsigned int _itemId;

public:
	unsigned int getItemId() { return _itemId; }
};
