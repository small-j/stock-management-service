#include "pch.h"
#include "StockManager.h"
#include "Stock.h"

// 새로 추가하는 Stock은 동적할당.
StockServer::StatusCode StockManager::addStock(const unsigned int itemId, const unsigned int count) {
	if (count == 0) return StockServer::StatusCode::CANCELLED;
	
	auto stockItr = _stocks.find(itemId);
	if (stockItr == _stocks.end()) {
		// add new stock
		if (std::shared_ptr<Stock> stockSp = std::make_shared<Stock>(itemId, count)) {
			_stocks[itemId] = stockSp;
			return StockServer::StatusCode::OK;
		}
		return StockServer::StatusCode::CANCELLED;
	}

	std::shared_ptr<Stock> stockSp = stockItr->second;
	// itemId가 있는데 shared_ptr이 비어있는 경우는 에러 발생
	return stockSp->increaseCount(count);
}

// 삭제되는 Stock은 동적할당 해제.
StockServer::StatusCode StockManager::reduceStock(const unsigned int itemId, const unsigned int count) {
	if (count == 0) return StockServer::StatusCode::CANCELLED;
	
	auto stockItr = _stocks.find(itemId);
	if (stockItr == _stocks.end()) return StockServer::StatusCode::CANCELLED;

	if (std::shared_ptr<Stock> stock = stockItr->second) {
		if (stock->decreaseCount(count) != StockServer::StatusCode::OK) return StockServer::StatusCode::CANCELLED;

		if (stock->getCount() == 0) {
			_stocks.erase(stockItr);
			stock = nullptr;
		}

		return StockServer::StatusCode::OK;
	}

	return StockServer::StatusCode::CANCELLED;
}

const std::shared_ptr<Stock> StockManager::findStockByItemId(const unsigned int itemId) const {
	auto stockItr = _stocks.find(itemId);
	if (stockItr == _stocks.end()) return nullptr;
	return stockItr->second;
}