#pragma once
#include "RequestCommand.h"
#include "BaseRequest.h"

class ReduceStockRequest : public BaseRequest {
public:
	ReduceStockRequest()
		:BaseRequest(Request::Command::REDUCE_STOCK) {
	}
	ReduceStockRequest(unsigned int itemId, unsigned int count)
		:BaseRequest(Request::Command::REDUCE_STOCK)
		, _itemId(itemId)
		, _count(count) {
	}

private:
	unsigned int _itemId;
	unsigned int _count;

public:
	unsigned int getItemId() const { return _itemId; }
	unsigned int getCount() const { return _count; }

	int serialize(char* buffer) const override;
	int deserialize(const char* buffer) override;
};
