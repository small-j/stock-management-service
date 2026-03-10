#pragma once
#include "BaseRequest.h"

class PrintItemRequest : public BaseRequest {
public:
	PrintItemRequest()
		:BaseRequest(Request::Command::PRINT_ITEM) {
	}
	virtual ~PrintItemRequest() = default;
};
