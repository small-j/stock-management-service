#pragma once
#include "BaseResponse.h"
#include "RequestCommand.h"

class PrintItemResponse : public BaseResponse {
public:
	PrintItemResponse()
		:BaseResponse(Request::Command::PRINT_ITEM, false) {
	}
	PrintItemResponse(bool status, std::string& message, std::string& itemList)
		:BaseResponse(Request::Command::PRINT_ITEM, status, message)
		, _itemList(itemList) {
	}
	virtual ~PrintItemResponse() = default;

private:
	std::string _itemList;

public:
	std::string getItemList() { return _itemList; }

	int serialize(char* buffer) override;
	int deserialize(const char* buffer) override;
};
