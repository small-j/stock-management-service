#pragma once
#include <iostream>
#include "ResponseCommand.h"
#include "BaseResponse.h"

class AddItemResponse : public BaseResponse {
public:
	AddItemResponse(bool status, std::string& message, std::string& itemInfo)
		:BaseResponse(ResponseCommand::ADD_ITEM, status, message)
		, _itemInfo(itemInfo) {
	}

private:
	std::string _itemInfo;
};
