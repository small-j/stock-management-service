#pragma once
#include "common.h"

class AddStockDto {
public:
	AddStockDto(unsigned int itemId, unsigned int count) : _itemId(itemId), _count(count) {}

private:
	unsigned int _itemId;
	unsigned int _count;

public:
	unsigned int getItemId() { return _itemId; }
	unsigned int getCount() { return _count; }
};
