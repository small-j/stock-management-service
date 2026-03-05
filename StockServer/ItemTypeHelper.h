#pragma once
#include <array>
#include <vector>

enum ItemType;

class ItemTypeHelper {
private:
    // itemtype과 수동으로 동기화해야함.
    inline static std::array<std::string, static_cast<size_t>(ItemType::COUNT)> enumToString = {
        "FOOD",
        "CLOTHES"
    };

public:
    static std::vector<std::string> getAllItemInfosToString() {
		std::vector<std::string> result;
        int index = 1;
        for (const auto& str : enumToString) {
            result.push_back(std::to_string(index) + ". " + str);
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