#pragma once

using namespace std;

// 다음 사람이 나만큼 코드를 모름. 그러니까 확장성 있고, 명확하게 알 수 있도록 짜야한다.
// 관련된 데이터는 뭉쳐서 선언하고.
// 제약을 많이 둬야 문제가 발생되는걸 줄일 수 있다? 그래서 규칙을 명확하게 정한다면 명시도 명확하게.
enum ItemType;

// item은 수정될 수 없다. 무조건 삭제 아니면 추가.
class Item { 
	// 보통 클래스 구성
// 1. 생성자
// 2. 멤버변수
// 3. 함수

public:
	Item(const unsigned int id, const string& name, const ItemType& type);
	// virtual이 item을 상속받는 클래스가 있다면 필요함. 특별한 자료형이 멤버변수로 있는 경우가 아니라면 그냥 디폴트 소멸자 쓰자.
	// 요즘 추세는 소멸자에서 뭘 안하고 대신 finalize 같은 함수를 만들어서 거기서 처리.
	virtual ~Item() = default;

private:
	unsigned int _id;
	string _name;
	ItemType _type;

public:
	static constexpr unsigned int INVALID_ID = 0;

public:
	// getter, setter는 헤더파일에 많이 있음.
	// getter는 보통 const로 만듦. 멤버변수도 const로 만들어야함. // 마지막 const는 이걸 사용하는 곳에서 멤버변수를 수정하지 않는다는 뜻.

	const unsigned int getId() const;
	const string& getName() const;
	const ItemType& getType() const;

	bool isValid() const;
	const string toString() const;

	// valid, clear, toString?
	// id, name 비어있으면 안되고 type은 UNKNOWN이면 안됨.
	// toString -> item 정보 출력용.
};