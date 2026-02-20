#pragma once
#include <iostream>
#include "RequestCommand.h"
#include "BaseRequest.h"

class PrintItemRequest : public BaseRequest {
public:
	PrintItemRequest()
		:BaseRequest(RequestCommand::PRINT_ITEM) {
	}
};
