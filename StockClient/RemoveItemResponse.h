#pragma once
#pragma once
#include "BaseResponse.h"

class RemoveItemResponse : public BaseResponse {
public:
	RemoveItemResponse()
		:BaseResponse(false) {
	}
	RemoveItemResponse(bool status, std::string& message)
		:BaseResponse(status, message) {
	}
};
