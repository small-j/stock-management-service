#pragma once
#include <iostream>

#define RES_STATUS_SIZE 2 // 0: fail 1: success
// TODO: 한글 주석 warning 조사

class BaseResponse {
public:
	BaseResponse(bool status, const std::string& message = std::string())
		:_status(status)
		, _message(message) {
	}

protected:
	bool _status;
	std::string _message;

public:
	bool getStatus() { return _status; }
	std::string getMessage() { return _message;  }

	virtual int serialize(char* buffer);
	virtual int deserialize(const char* buffer);
};
