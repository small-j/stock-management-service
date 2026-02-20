#pragma on#pragma once
#include <iostream>
#include "RequestCommand.h"
#include "BaseRequest.h"

class RemoveItemRequest : public BaseRequest {
public:
	RemoveItemRequest(unsigned int itemId)
		:BaseRequest(RequestCommand::REMOVE_ITEM)
		, _itemId(itemId) {
	}

private:
	unsigned int _itemId;
};