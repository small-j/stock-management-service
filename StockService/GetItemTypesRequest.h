#pragma once
#include "BaseRequest.h"

class GetItemTypesRequest : public BaseRequest {
public:
	GetItemTypesRequest()
		:BaseRequest(Request::Command::GET_ITEM_TYPE) {
	}
};
