#pragma once
#include "BaseResponse.h"
#include "RequestCommand.h"

class AddItemResponse : public BaseResponse {
public:
	AddItemResponse()
		:BaseResponse(Request::Command::ADD_ITEM, false) {
	}
	AddItemResponse(bool status, std::string& message)
		:BaseResponse(Request::Command::ADD_ITEM, status, message) {
	}
};
