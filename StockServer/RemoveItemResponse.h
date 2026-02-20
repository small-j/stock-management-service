#pragma once
#include <iostream>
#include "ResponseCommand.h"
#include "BaseResponse.h"

class RemoveItemResponse : public BaseResponse {
public:
	RemoveItemResponse(bool status, std::string& message, std::string& itemInfo)
		:BaseResponse(ResponseCommand::REMOVE_ITEM, status, message)
		, _itemInfo(itemInfo) {
	}

private:
	std::string _itemInfo;
};
