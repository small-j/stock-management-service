#include "pch.h"
#include "GetItemTypesResponse.h"

std::string GetItemTypesResponse::toString() {
	std::string result = "사용 가능한 아이템 타입:\n"; // TODO : 한국어 인코딩 고치기.
	for (size_t i = 0; i < _itemTypes.size(); ++i) {
		result += _itemTypes[i] + "\n";
	}
	return result;
}

int GetItemTypesResponse::serialize(char* buffer) {
	int offset = __super::serialize(buffer);

	char delimiter = ',';
	int delimiterSize = sizeof(delimiter);

	int strLength = (_itemTypes.size() - 1) * delimiterSize;
	for (std::string& menu : _itemTypes) {
		strLength += static_cast<int>(menu.length());
	}

	// 문자열 길이 기록.
	memcpy(buffer + offset, &strLength, sizeof(int));
	offset += sizeof(int);

	// 문자열 기록.
	for (size_t i = 0; i < _itemTypes.size(); ++i) {
		std::string temp = i == _itemTypes.size() - 1 ? _itemTypes[i] : _itemTypes[i] + delimiter;
		memcpy(buffer + offset, temp.c_str(), temp.length());
		offset += static_cast<int>(temp.length());
	}

	return offset;
}

int GetItemTypesResponse::deserialize(const char* buffer) {
	int offset = __super::deserialize(buffer);

	int strLength = 0;
	memcpy(&strLength, buffer + offset, sizeof(int));
	offset += sizeof(int);

	const std::string itemTypes(buffer + offset);
	char delimiter = ',';
	size_t start = 0;
	size_t end = itemTypes.find(delimiter);

	while (end != std::string::npos) {
		_itemTypes.push_back(itemTypes.substr(start, end - start));
		start = end + 1; // delimiter 길이만큼 더한다.
		end = itemTypes.find(delimiter, start);
	}
	_itemTypes.push_back(itemTypes.substr(start));

	return offset;
}