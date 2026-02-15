#include "pch.h"
#include "StockManager.h"
#include "Stock.h"

// 새로 추가하는 Stock은 동적할당. !!new는 동적할당인가?
bool StockManager::addStock(const unsigned int itemId, const unsigned int count) {
	if (count == 0) return false;
	
	auto stockItr = _stocks.find(itemId);
	if (stockItr == _stocks.end()) {
		// add new stock
		if (shared_ptr<Stock> stockSp = make_shared<Stock>(itemId, count)) {
			// shared_ptr *Sp
			// pointer *Ptr
			_stocks[itemId] = stockSp;
			return true;
		}
		return false;
	}

	shared_ptr<Stock> stockSp = stockItr->second;
	return stockSp->increaseCount(count);
	// 일부로 exception 터트리는 경우도 있다
	// itemId가 있는데 shared_ptr이 비어있는 경우는 진짜 이슈다. 그래서 이거는 터지는게 맞다.
}

// 삭제되는 Stock은 동적할당 해제.
bool StockManager::reduceStock(const unsigned int itemId, const unsigned int count) {
	if (count == 0) return false;
	
	auto stockItr = _stocks.find(itemId);
	if (stockItr == _stocks.end()) return false;

	if (shared_ptr<Stock> stock = stockItr->second) {
		if (stock->decreaseCount(count) != true) return false;

		if (stock->getCount() == 0) {
			_stocks.erase(stockItr);
			stock = nullptr;
		}

		return true;
	}

	return false;
}

const shared_ptr<Stock> StockManager::findStockByItemId(const unsigned int itemId) const {
	auto stockItr = _stocks.find(itemId);
	if (stockItr == _stocks.end()) return nullptr;
	return stockItr->second;
}