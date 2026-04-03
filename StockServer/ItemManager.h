#pragma once

#include "common.h"

class Item;

typedef std::pair<unsigned int, std::shared_ptr<Item> > ItemSearchPair; // TODO: 이거 어디서 쓰이는지?

class ItemManager {
public:
	ItemManager() = default;
	virtual ~ItemManager() = default;

private:
	std::map<unsigned int, std::shared_ptr<Item> > _items;

private:
	const unsigned int publishItemId();

public:
	void finalize(); // item 전체 동적할당 해제.
	// item 동적할당. 여기서 아이디 발급 실패시 예외처리.
	StockServer::StatusCode addItem(const std::string& name, const ItemType& type);
	// 동적할당 해제. stock과 연결된 item은 삭제 불가.
	StockServer::StatusCode removeItem(const unsigned int id);
	const std::shared_ptr<Item> findItemById(const unsigned int itemId) const;
	std::string itemListToString() const;
};