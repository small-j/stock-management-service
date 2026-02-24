#pragma once
#include "BaseResponse.h"
#include <vector>

class GetItemTypesResponse : public BaseResponse {
public:
	GetItemTypesResponse()
		:BaseResponse(false) {
	}
	GetItemTypesResponse(bool status, std::string& message, std::string& itemTypes)
		:BaseResponse(status, message) {

		char delimiter = ',';
		size_t start = 0;
		size_t end = itemTypes.find(delimiter);

		while (end != std::string::npos) {
			_itemTypes.push_back(itemTypes.substr(start, end - start));
			start = end + 1; // delimiter 길이만큼 더한다.
			end = itemTypes.find(delimiter, start);
		}
		_itemTypes.push_back(itemTypes.substr(start));
	}

private:
	std::vector<std::string> _itemTypes;

public:
	std::vector<std::string> getMenus() { return _itemTypes; }
	std::string toString();

	int serialize(char* buffer) override;
	int deserialize(const char* buffer) override;
};
