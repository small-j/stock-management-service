#pragma once
#include <iostream>
#include "RequestCommand.h"
#include "BaseRequest.h"

class AddItemRequest : public BaseRequest {
public:
	AddItemRequest(std::string& name, int itemType)
		:BaseRequest(RequestCommand::ADD_ITEM)
		, _name(name)
		, _itemType(itemType) {
	}

private:
	std::string _name;
	int _itemType;
};
