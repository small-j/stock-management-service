#pragma once
#include "BaseResponse.h"

class PrintItemResponse : public BaseResponse {
public:
	PrintItemResponse()
		:BaseResponse(false) {
	}
	PrintItemResponse(bool status, std::string& message, std::string& itemList)
		:BaseResponse(status, message)
		, _itemList(itemList) {
	}

private:
	std::string _itemList;

public:
	std::string getItemList() { return _itemList; }

	int serialize(char* buffer) override;
	int deserialize(const char* buffer) override;
};
