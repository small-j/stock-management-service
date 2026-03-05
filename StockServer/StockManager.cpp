#include "pch.h"
#include "StockManager.h"
#include "Stock.h"

// 새로 추가하는 Stock은 동적할당.
bool StockManager::addStock(const unsigned int itemId, const unsigned int count) {
	if (count == 0) return false;
	
	auto stockItr = _stocks.find(itemId);
	if (stockItr == _stocks.end()) {
		// add new stock
		if (shared_ptr<Stock> stockSp = make_shared<Stock>(itemId, count)) {
			_stocks[itemId] = stockSp;
			return true;
		}
		return false;
	}

	shared_ptr<Stock> stockSp = stockItr->second;
	// itemId가 있는데 shared_ptr이 비어있는 경우는 에러 발생
	return stockSp->increaseCount(count);
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