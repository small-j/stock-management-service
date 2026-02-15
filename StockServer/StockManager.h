#pragma once
#include <map>

using namespace std;

class Stock;
// 포인터로 썼을 떄만 전방선언 가능. 참조형으로 받으면 include 해야함.
// 규모가 큰 프로젝트는 최대한 include를 줄이는게 좋음.

class StockManager {
public:
	StockManager() = default;
	virtual ~StockManager() = default;

private:
	// <item_id, stock_ptr>
	map<unsigned int, shared_ptr<Stock> > _stocks; // shared_ptr을 typedef 로 빼면 꺽쇄 붙여도 됨.
	//vector<StockHistory> _stockHistory; // 2차 구현 예정

public:
	// 새로 추가하는 Stock은 동적할당. !!new는 동적할당인가?
	bool addStock(const unsigned int itemId, const unsigned int count);
	// 삭제되는 Stock은 동적할당 해제.
	bool reduceStock(const unsigned int itemId, const unsigned int count);
	const shared_ptr<Stock> findStockByItemId(const unsigned int itemId) const;

	//bool importStock(csv); // 2차 구현 csv 파일 받을 수 있는 라이브러리? 찾아보기.
};