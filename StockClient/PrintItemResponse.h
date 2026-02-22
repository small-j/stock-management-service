#pragma once
#include "BaseResponse.h"

// inteligence가 문법 보고 알아서 이쁘게 , 혹은 에러 보여주는, 그래서 예전 에러 보여주는 경우가 있음.
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

	// int* offset -> 부모 클래스의offset, 반환받거나
	// override
	int serialize(char* buffer) override;
	int deserialize(const char* buffer) override;
};
