#pragma once
#include "BaseResponse.h"

class ReduceStockResponse : public BaseResponse {
public:
	ReduceStockResponse()
		:BaseResponse(false) {
	}
	ReduceStockResponse(bool status, std::string& message)
		:BaseResponse(status, message) {
	}
};
