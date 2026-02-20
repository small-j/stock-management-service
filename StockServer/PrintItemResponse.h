#pragma once
#include <iostream>
#include "ResponseCommand.h"
#include "BaseResponse.h"

class PrintItemResponse : public BaseResponse {
public:
	PrintItemResponse(bool status, std::string& message, std::string& itemList)
		:BaseResponse(ResponseCommand::PRINT_ITEM, status, message)
		, _itemList(itemList){
	}

private:
	std::string _itemList;
};
