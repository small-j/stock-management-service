#include "GetItemTypesResponse.h"

std::string GetItemTypesResponse::toString() {
	std::string result = "사용 가능한 아이템 타입:\n"; // TODO : 한국어 인코딩 고치기
	for (size_t i = 0; i < _itemTypes.size(); ++i) {
		result += _itemTypes[i] + "\n";
	}
	return result;
}

int GetItemTypesResponse::serialize(char* buffer) {
	int offset = __super::serialize(buffer);
	int strLength = 0;

	for (std::vector < std::string > ::iterator iter = _itemTypes.begin(); iter != _itemTypes.end(); ++iter) {
		strLength += static_cast<int>((*iter).length());
	}

	memcpy(buffer + offset, &strLength, sizeof(int));
	offset += sizeof(int);

	for (size_t i = 0; i < _itemTypes.size(); ++i) {
		memcpy(buffer + offset, _itemTypes[i].c_str(), _itemTypes[i].size());
		offset += static_cast<int>(_itemTypes[i].length());
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