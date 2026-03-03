
#pragma once
#include "RequestCommand.h"
#include "BaseRequest.h"

class AddStockRequest : public BaseRequest {
public:
	AddStockRequest()
		:BaseRequest(Request::Command::ADD_STOCK) {
	}
	AddStockRequest(unsigned int itemId, unsigned int count)
		:BaseRequest(Request::Command::ADD_STOCK)
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
