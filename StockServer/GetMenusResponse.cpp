#include "pch.h"
#include "GetMenusResponse.h"

std::string GetMenusResponse::toString() {
	std::string result;
	for (size_t i = 0; i < _menus.size(); ++i) {
		result += _menus[i] + "\n";
	}
	return result;
}

int GetMenusResponse::serialize(char* buffer) {
	int offset = __super::serialize(buffer);
	
	char delimiter = ',';
	int delimiterSize = static_cast<int>(sizeof(delimiter));

	int strLength = (_menus.size() - 1) * delimiterSize;
	for (std::string& menu : _menus) {
		strLength += static_cast<int>(menu.length());
	}

	// 문자열 길이 기록
	memcpy(buffer + offset, &strLength, sizeof(int));
	offset += sizeof(int);

	// 문자열 기록
	for (size_t i = 0; i < _menus.size(); ++i) {
		std::string temp = i == _menus.size() - 1 ? _menus[i] : _menus[i] + delimiter;
		memcpy(buffer + offset, temp.c_str(), temp.length());
		offset += static_cast<int>(temp.length());
	}

	return offset;
}

int GetMenusResponse::deserialize(const char* buffer) {
	int offset = __super::deserialize(buffer);

	int strLength = 0;
	memcpy(&strLength, buffer + offset, sizeof(int));
	offset += sizeof(int);

	const std::string menus(buffer + offset);
	char delimiter = ',';
	size_t start = 0;
	size_t end = menus.find(delimiter);

	while (end != std::string::npos) {
		_menus.push_back(menus.substr(start, end - start));
		start = end + 1; // delimiter 길이만큼 더한다.
		end = menus.find(delimiter, start);
	}
	_menus.push_back(menus.substr(start));

	return offset;
}