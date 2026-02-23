#pragma once
#include "RequestCommand.h"
#include "BaseRequest.h"

class BaseRequest;

class AddItemRequest : public BaseRequest {
public:
	AddItemRequest(const std::string& name, const int itemType)
		:BaseRequest(Request::Command::ADD_ITEM)
		, _name(name)
		, _itemType(itemType) {
	}

private:
	std::string _name;
	int _itemType;

public:
	int serialize(char* buffer) override;
	int deserialize(const char* buffer) override;
};
