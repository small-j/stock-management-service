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
}

// item 동적할당. 여기서 아이디 발급 실패시 예외처리.
StockServer::StatusCode ItemManager::addItem(const string& name, const ItemType& type) {
	if (name.length() == 0) return StockServer::StatusCode::CANCELLED;
	if (ItemTypeHelper::isValid(type) == false) return StockServer::StatusCode::CANCELLED;

	unsigned int id = publishItemId();
	if (id == Item::INVALID_ID) return StockServer::StatusCode::CANCELLED;

	// TODO: 할당이 안되는 경우 처리 필요.
	shared_ptr<Item> itemSp = make_shared<Item>(id, name, type);

	if (itemSp.get() == nullptr || !itemSp->isValid()) return StockServer::StatusCode::CANCELLED;

	_items[id] = itemSp;

	return StockServer::StatusCode::OK;
}

// 동적할당 해제. stock과 연결된 item은 삭제 불가.
// 현재는 의존성이 약해질수록 상위에서 처리해주어야 하는 로직, 작업이 많아지고 사용 방법이 어려워짐.
StockServer::StatusCode ItemManager::removeItem(const unsigned int itemId) {
	auto itemItr = _items.find(itemId);
	if (_items.end() == itemItr) return StockServer::StatusCode::CANCELLED; // not found item id. already removed.

	_items.erase(itemItr);
	return StockServer::StatusCode::OK;
}

const shared_ptr<Item> ItemManager::findItemById(const unsigned int itemId) const {
	//map search 

	auto itemItr = _items.find(itemId);
	if (itemItr == _items.end()) return nullptr;
	return itemItr->second;
}

std::string ItemManager::itemListToString() const
{
	string result;
	for (const auto& [id, item] : _items) {
		result += item->toString() + "\n";
	}

	return result;
}