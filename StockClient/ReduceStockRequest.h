#pragma once
#include "RequestCommand.h"
#include "BaseRequest.h"

class ReduceStockRequest : public BaseRequest {
public:
	ReduceStockRequest(unsigned int itemId, unsigned int count)
		:BaseRequest(Request::Command::REDUCE_STOCK)
		, _itemId(itemId)
		, _count(count) {
	}

private:
	unsigned int _itemId;
	unsigned int _count;

public:
	int serialize(char* buffer) override;
	int deserialize(const char* buffer) override;
};
