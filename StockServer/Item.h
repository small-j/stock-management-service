#pragma once

using namespace std;

// 다음 사람이 나만큼 코드를 모름. 그러니까 확장성 있고, 명확하게 알 수 있도록 짜야한다.
// 관련된 데이터는 뭉쳐서 선언하고.
// 제약을 많이 둬야 문제가 발생되는걸 줄일 수 있다? 그래서 규칙을 명확하게 정한다면 명시도 명확하게.
enum ItemType;
class Item { 

public:
	Item(const unsigned int id, const string& name, const ItemType& type);
	virtual ~Item() = default;

private:
	unsigned int _id;
	string _name;
	ItemType _type;

public:
	static constexpr unsigned int INVALID_ID = 0;

public:
	const unsigned int getId() const;
	const string& getName() const;
	const ItemType& getType() const;

	bool isValid() const;
	const string toString() const;
};