#include "pch.h"
#include "Stock.h"
#include "Item.h"

Stock::Stock(const unsigned int itemId, const unsigned int count)
: _itemId(itemId), _count(count) {}

const unsigned int Stock::getItemId() const {
	return _itemId;
}

const unsigned int Stock::getCount() const {
	return _count;
}

bool Stock::increaseCount(const unsigned int count) {
	unsigned int maxCount = std::numeric_limits<unsigned int>::max();
	if (maxCount - count < _count) return false;
	
	_count += count;
	return true;
}

bool Stock::decreaseCount(const unsigned int count) {
	if (_count < count) return false;

	_count -= count;
	return true;
}


bool Stock::isValid() const {
	if (_itemId == Item::INVALID_ID) return false;
	if (_count > COUNT_MAX) return false;
}