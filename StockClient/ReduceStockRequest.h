#pragma once
#include <iostream>
#include "RequestCommand.h"
#include "BaseRequest.h"

class ReduceStockRequest : public BaseRequest {
public:
	ReduceStockRequest(unsigned int itemId, unsigned int count)
		:BaseRequest(RequestCommand::REDUCE_STOCK)
		, _itemId(itemId)
		, _count(count) {
	}

private:
	unsigned int _itemId;
	unsigned int _count;
};
