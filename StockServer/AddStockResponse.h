#pragma once
#include <iostream>
#include "ResponseCommand.h"
#include "BaseResponse.h"

class AddStockResponse : public BaseResponse {
public:
	AddStockResponse(bool status, std::string& message, std::string& stockInfo)
		:BaseResponse(ResponseCommand::ADD_STOCK, status, message)
		, _stockInfo(stockInfo) {
	}

private:
	std::string _stockInfo;
};
