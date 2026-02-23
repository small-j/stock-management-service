#pragma once
#include "pch.h"

class BaseResponse {
public:
	BaseResponse(bool status, std::string& message)
		:_status(status)
		,_message(message) {
	}

private:
	bool _status;
	std::string _message;
};
