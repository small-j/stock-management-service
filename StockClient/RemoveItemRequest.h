#pragma once
#include "RequestCommand.h"
#include "BaseRequest.h"

class RemoveItemRequest : public BaseRequest {
public:
	RemoveItemRequest(unsigned int itemId)
		:BaseRequest(Request::Command::REMOVE_ITEM)
		, _itemId(itemId) {
	}

private:
	unsigned int _itemId;

public:
	int serialize(char* buffer) override;
	int deserialize(const char* buffer) override;
};