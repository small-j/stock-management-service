#pragma once

enum ItemType;
class Item { 

public:
	Item(const unsigned int id, const std::string& name, const ItemType& type);
	virtual ~Item() = default;

private:
	unsigned int _id;
	std::string _name;
	ItemType _type;

public:
	static constexpr unsigned int INVALID_ID = 0;

public:
	const unsigned int getId() const;
	const std::string& getName() const;
	const ItemType& getType() const;

	bool isValid() const;
	const std::string toString() const;
};