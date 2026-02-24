#pragma once
#include "BaseResponse.h"

class AddStockResponse : public BaseResponse {
public:
	AddStockResponse()
		:BaseResponse(false) {
	}
	AddStockResponse(bool status, std::string& message)
		:BaseResponse(status, message) {
	}
};
