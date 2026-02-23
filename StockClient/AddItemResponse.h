#pragma once
#include "BaseResponse.h"

class AddItemResponse : public BaseResponse {
public:
	AddItemResponse()
		:BaseResponse(false) {
	}
	AddItemResponse(bool status, std::string& message)
		:BaseResponse(status, message) {
	}
};
