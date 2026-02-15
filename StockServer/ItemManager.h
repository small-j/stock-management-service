#pragma once
#include <map>

using namespace std;

class Item;

typedef pair<unsigned int, shared_ptr<Item> > ItemSearchPair;

// 가정 : 내가 가고자 하는 회사들은 대체로 사용자는 적고 데이터는 클 것임.
// 큰 데이터를 빠르고 효율적으로 관리하는 것이 목표.
// UUID의 사용 목적 : 어떤 경우에도 유니크한 key 발급. 즉, 동시다발적으로 처리되는 작업에서 요청 단위를 분리하기 위함에서 많이 씀.
// 중앙화 되지 않은 환경에서 요청을 식별하기 위함.
// 하지만 검색에는 최적화 x
// 내 서비스는 유저가 많지 않으니(여러 요청이 동시다발 발생x 및 구분 불필요) UUID는 불필요.
// 대신 데이터가 대량으로 입고, 출고 될 수 있으니 빠르게 item, stock 을 검색하는게 필요하다고 생각.
// 일단 그만 고민하고 map으로 저장할까?ㅋㅋㅋ
class ItemManager {
public:
	ItemManager() = default;
	virtual ~ItemManager() = default;

private:
	//unsigned int _lastIds; // lock 필요/
	map<unsigned int, shared_ptr<Item> > _items;
	// 간단한 자료형은 효율보다는 직관적인게 낫다.
	// 초기에 없는 구조를 만들때는 일단 직관적으로 짜고
	// 나중에 효율적으로 고쳐야할 필요가 있을 때 고치자.
	// 그러면 구조를 몇점까지 짜고나서 나중에 고쳐도 되는가?

private:
	const unsigned int publishItemId();

public:
	void finalize(); // item 전체 동적할당 해제.
	// item 동적할당. 여기서 아이디 발급 실패시 예외처리.
	bool addItem(const string& name, const ItemType& type);
	// 동적할당 해제. stock과 연결된 item은 삭제 불가.
	bool removeItem(const unsigned int id);
	// 쉐어드 포인터로 반환하는 것과 쉐어드 포인터의 참조형으로 반환하는 것과 다름
	// -> item을 수정할일이 있으면 참조형으로 반환해야함.
	const shared_ptr<Item> findItemById(const unsigned int itemId) const;
	string itemListToString() const;
};