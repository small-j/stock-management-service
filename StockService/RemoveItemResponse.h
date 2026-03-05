#pragma once
#pragma once
#include "BaseResponse.h"
#include "RequestCommand.h"

class RemoveItemResponse : public BaseResponse {
public:
	RemoveItemResponse()
		:BaseResponse(Request::Command::REMOVE_ITEM, false) {
	}
	RemoveItemResponse(bool status, std::string& message)
		:BaseResponse(Request::Command::REMOVE_ITEM, status, message) {
	}
};
