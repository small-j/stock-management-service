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
	int strLength = 0;

	for (std::vector < std::string > ::iterator iter = _menus.begin(); iter != _menus.end(); ++iter) {
		strLength += static_cast<int>((*iter).length());
	}

	memcpy(buffer, &strLength, sizeof(int));
	offset += sizeof(int);

	for (size_t i = 0; i < _menus.size(); ++i) {
		memcpy(buffer + offset, _menus[i].c_str(), _menus[i].size());
		offset += static_cast<int>(_menus[i].length());
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