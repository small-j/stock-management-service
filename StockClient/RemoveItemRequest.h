#pragma once
#include "RequestCommand.h"
#include "BaseRequest.h"

class RemoveItemRequest : public BaseRequest {
public:
	RemoveItemRequest()
		:BaseRequest(Request::Command::REMOVE_ITEM) {
	}
	RemoveItemRequest(unsigned int itemId)
		:BaseRequest(Request::Command::REMOVE_ITEM)
		, _itemId(itemId) {
	}

private:
	unsigned int _itemId;

public:
	unsigned int getItemId() const { return _itemId; }

	int serialize(char* buffer) const override;
	int deserialize(const char* buffer) override;
};