#include "pch.h"
#include <format>
#include "Item.h"
#include "ItemTypeHelper.h"

//Item::Item()
//	: _id(0)
//	, _name()
//	, type(ItemType::UNKNOWN)
//{
//}

Item::Item(const unsigned int id, const string& name, const ItemType& type) // 화면상에서는 마이너스를 받아서 처리하도록.
	: _id(id), _name(name), _type(type) {}

const unsigned int Item::getId() const {
	return _id;
}

const string& Item::getName() const {
	return _name;
}

const ItemType& Item::getType() const {
	return _type;
}

bool Item::isValid() const {
	if (_id == Item::INVALID_ID) return false;
	if (_name.empty()) return false;
	if (_type <= ItemType::UNKNOWN && ItemType::COUNT <= _type) return false;
	return true;
}

const string Item::toString() const {
	string enumToString;
	if (ItemTypeHelper::toString(_type, &enumToString)) {
		return std::format("Item ID: {}, Name: {}, Type: {}", _id, _name, enumToString);
	}
	else {
		throw::runtime_error("Item::toString() - ItemType to string conversion failed.");
	}

	return "";
}