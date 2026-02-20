#pragma once
#include <iostream>
#include "ResponseCommand.h"
#include "BaseResponse.h"

class ReduceStockResponse : public BaseResponse {
public:
	ReduceStockResponse(bool status, std::string& message, std::string& stockInfo)
		:BaseResponse(ResponseCommand::REDUCE_STOCK, status, message)
		, _stockInfo(stockInfo) {
	}

private:
	std::string _stockInfo;
};
