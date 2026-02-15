#pragma once
#include <string>

// 攫力 立加, 攫力 
enum CommandCode {
	UNKNOWN = -1,
	SHOW_ITEM_LIST = 0,
	ADD_ITEM = 1,
	REMOVE_ITEM = 2,
	ADD_STOCK = 3,
	REDUCE_STOCK = 4,
	COUNT = 5
};

class BaseProtocol {
public:
	BaseProtocol(CommandCode commandCode, int dataSize)
		: _commandCode(commandCode), _dataSize(dataSize) {}
	virtual ~BaseProtocol() = default;

	CommandCode _commandCode;
	int _dataSize;
};

enum ErrorCode {
	UNKNOWN_ERROR = -1,
	INVALID_REQUEST = 0,
	NOT_FOUND = 1,
	SERVER_ERROR = 2,
	COUNT = 3
};

class ErrorProtocol {
public:
	ErrorProtocol(ErrorCode errorCode, const std::string& errorMessage)
		: _errorCode(errorCode),
		_errorMessage(errorMessage) {}
	virtual ~ErrorProtocol() = default;

	ErrorCode _errorCode;
	std::string _errorMessage;
};

class AddItemProtocol : public BaseProtocol {
public:
	AddItemProtocol(CommandCode commandCode, int dataSize, const std::string itemName) 
		:BaseProtocol(commandCode, dataSize),
		_itemName(itemName),
		_itemType(0) {}
	~AddItemProtocol() override = default;
	
	std::string _itemName;
	int _itemType;
};

class RemoveItemProtocol : public BaseProtocol {
public:
	RemoveItemProtocol() = default;
	~RemoveItemProtocol() override = default;
	unsigned int _itemId;
};

class AddStockProtocol : public BaseProtocol {
public:
	AddStockProtocol() = default;
	~AddStockProtocol() override = default;
	
	unsigned int _itemId;
	unsigned int _count;
};

class ReduceStockProtocol : public BaseProtocol {
public:
	ReduceStockProtocol() = default;
	~ReduceStockProtocol() override = default;

	unsigned int _itemId;
	unsigned int _count;
};
