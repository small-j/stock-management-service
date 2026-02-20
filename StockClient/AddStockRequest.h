
#pragma once
#include <iostream>
#include "RequestCommand.h"
#include "BaseRequest.h"

class AddStockRequest : public BaseRequest {
public:
	AddStockRequest(unsigned int itemId, unsigned int count)
		:BaseRequest(RequestCommand::ADD_STOCK)
		, _itemId(itemId)
		, _count(count) {
	}

private:
	unsigned int _itemId;
	unsigned int _count;
};
