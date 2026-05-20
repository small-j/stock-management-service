#pragma once

#include "common.h"

class AddItemDto {
public:
	AddItemDto(std::string name, short itemType) : _name(name), _itemType(itemType) {}

private:
	std::string _name;
	short _itemType;

public:
	std::string getName() { return _name; }
	short getItemType() { return _itemType; }
};
