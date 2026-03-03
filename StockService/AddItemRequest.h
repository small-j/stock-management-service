#pragma once
#include "RequestCommand.h"
#include "BaseRequest.h"

class BaseRequest;

class AddItemRequest : public BaseRequest {
public:
	AddItemRequest()
		:BaseRequest(Request::Command::ADD_ITEM) {
	}
	AddItemRequest(const std::string& name, const int itemType)
		:BaseRequest(Request::Command::ADD_ITEM)
		, _name(name)
		, _itemType(itemType) {
	}

private:
	std::string _name;
	int _itemType;

public:
	std::string getName() const { return _name; }
	int getItemType() const { return _itemType; }

	int serialize(char* buffer) const override;
	int deserialize(const char* buffer) override;
};
