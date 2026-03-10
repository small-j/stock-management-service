#pragma once
#include "BaseResponse.h"
#include "RequestCommand.h"
#include <vector>

class GetMenusResponse : public BaseResponse {
public:
	GetMenusResponse()
		:BaseResponse(Request::Command::GET_MENU, false) {
	}
	GetMenusResponse(bool status, std::string& message, std::string& menus)
		:BaseResponse(Request::Command::GET_MENU, status, message) {
		
		char delimiter = ',';
		size_t start = 0;
		size_t end = menus.find(delimiter);

		while (end != std::string::npos) {
			_menus.push_back(menus.substr(start, end - start));
			start = end + 1; // delimiter 길이만큼 더한다.
			end = menus.find(delimiter, start);
		}
		_menus.push_back(menus.substr(start));
	}
	virtual ~GetMenusResponse() = default;

private:
	std::vector<std::string> _menus;

public:
	std::vector<std::string> getMenus() { return _menus; }
	std::string toString();

	int serialize(char* buffer) override;
	int deserialize(const char* buffer) override;
};
