#pragma once
#include "BaseResponse.h"
#include "RequestCommand.h"

class AddStockResponse : public BaseResponse {
public:
	AddStockResponse()
		:BaseResponse(Request::Command::ADD_STOCK, false) {
	}
	AddStockResponse(bool status, std::string& message)
		:BaseResponse(Request::Command::ADD_STOCK, status, message) {
	}
};
