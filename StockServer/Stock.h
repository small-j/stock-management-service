#pragma once

// count가 0이면 stock이 없어진다.
class Stock {
public:
	Stock(const unsigned int itemId, const unsigned int count);
	virtual ~Stock() = default;
private:
	//static은 보통 다 대문자.
	static constexpr unsigned int COUNT_MAX = 4000000000;
	unsigned int _itemId;
	unsigned int _count; // 외상을 사용하지 않기 때문에 count는 음수일 수 없다.

public:
	const unsigned int getItemId() const;
	const unsigned int getCount() const;
	StockServer::StatusCode increaseCount(const unsigned int count);
	StockServer::StatusCode decreaseCount(const unsigned int count);

	bool isValid() const;
};