#pragma once
#include "BaseResponse.h"
#include "RequestCommand.h"

class ReduceStockResponse : public BaseResponse {
public:
	ReduceStockResponse()
		:BaseResponse(Request::Command::REDUCE_STOCK, false) {
	}
	ReduceStockResponse(bool status, std::string& message)
		:BaseResponse(Request::Command::REDUCE_STOCK, status, message) {
	}
};
