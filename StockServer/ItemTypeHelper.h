#pragma once
#include <array>

enum ItemType;

class ItemTypeHelper {
    // TODO: 둘중 뭐가 좋은지 알아보기.
private:
    //inline static const string enumToString[ItemType::COUNT] = { // string 형태의 배열. 인덱스 몇번까지 지정하지 않아도 문제 없음.
    //    "FOOD",
    //    "CLOTHES"
    //};

    inline static std::array<std::string, static_cast<size_t>(ItemType::COUNT)> enumToString = { // itemtype과 수동으로 동기화해야함.
        "FOOD",
        "CLOTHES"
	};

public:
    static std::string getAllItemInfoToString() {
        std::string result =  "사용 가능한 아이템 타입:\n";
        int index = 1;
        for (const auto& str : enumToString) {
            result += std::to_string(index) + ". " + str + "\n";
            index++;
        }

        return result;
    }

    static bool toString(ItemType type, std::string* result) {
        if (type <= ItemType::UNKNOWN || ItemType::COUNT <= type) return false;
        *result = enumToString[type];
        return result->empty() != true;
    }

    static bool isValid(ItemType type) {
        if (type <= ItemType::UNKNOWN || ItemType::COUNT <= type) return false;
        return true;
    }
};