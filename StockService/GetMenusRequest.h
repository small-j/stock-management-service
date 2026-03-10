#pragma once
#include "BaseRequest.h"

class GetMenusRequest : public BaseRequest {
public:
	GetMenusRequest()
		:BaseRequest(Request::Command::GET_MENU) {
	}
	virtual ~GetMenusRequest() = default;
};
