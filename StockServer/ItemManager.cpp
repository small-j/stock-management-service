#include "pch.h"
#include "ItemManager.h"
#include "Item.h"
#include "ItemTypeHelper.h"

const unsigned int ItemManager::publishItemId() {
	for (int idx = 1; idx < 10000; ++idx) {
		if (_items.end() == _items.find(idx)) {
			return idx;
		}
	}
	return Item::INVALID_ID;

	// try catch는 로직 실행을 쭉 내려가게 되어있지 않아서 잘 꼭 필요한 경우에만 쓴다.
}

// item 동적할당. 여기서 아이디 발급 실패시 예외처리.
bool ItemManager::addItem(const string& name, const ItemType& type) {
	if (name.length() == 0) return false;
	if (ItemTypeHelper::isValid(type) == false) return false;

	unsigned int id = publishItemId();
	if (id == Item::INVALID_ID) return false;

	shared_ptr<Item> itemSp = make_shared<Item>(id, name, type); // 하다가 할당이 안될수도 있음.

	if (itemSp.get() == nullptr || !itemSp->isValid()) return false;

	_items[id] = itemSp;

	return true;
}

// 동적할당 해제. stock과 연결된 item은 삭제 불가.
// item, stock manager는 무슨 관계인가? 동등한 관계?
// 의존성이 약해질수록 상위에서 처리해주어야 하는 로직, 작업이 많아지고 사용 방법이 어려워짐.
bool ItemManager::removeItem(const unsigned int itemId) {
	auto itemItr = _items.find(itemId);
	if (_items.end() == itemItr) return false; // not found item id. already removed.

	_items.erase(itemItr);
	return true;
}

// 쉐어드 포인터로 반환하는 것과 쉐어드 포인터의 참조형으로 반환하는 것과 다름
// -> item을 수정할일이 있으면 참조형으로 반환해야함.
const shared_ptr<Item> ItemManager::findItemById(const unsigned int itemId) const {
	//map search 

	auto itemItr = _items.find(itemId);
	if (itemItr == _items.end()) return nullptr;
	return itemItr->second;
}
// nullptr이 아니라 static으로 인벨리드한 아이템 하나 만들어서 반환해주는 경우도 있음. shared ptr을 못쓰는 경우.

std::string ItemManager::itemListToString() const
{
	string result;
	for (const auto& [id, item] : _items) {
		result += item->toString() + "\n";
	}

	return result;
}