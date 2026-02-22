#pragma once
#include <iostream>

#define RES_STATUS_SIZE 2 // 0: fail 1: success
#define RES_MESSAGE_SIZE 90 // 사용자에게 입력값 제한을 받아야 한다.
// 고정해도 되고, 메시지 사이즈를 주고 그 뒤에 메모리를 밀어넣어주는 방식도 있음.
// 각자 장단점이 있다. 보통 string은 한정을 두지 않음.
// 한글 주석 워닝 그냥 알아나보자

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
