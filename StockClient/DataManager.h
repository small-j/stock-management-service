#pragma once
#include <winsock.h>
#include <memory>

#define PACKET_SIZE 1024 // byte
// 1024로 고정되어 있어서 이 이상의 데이터를 보낼 수가 없다.
// 어떻게 고쳐야할지 모르겠는데 
// 지금도 고쳐야할게 많으니까 나중에 추가 공부를 해보자

class BaseRequest;
class BaseResponse;

class DataManager {
public:
	void sendToServer(SOCKET& serverSocket, BaseRequest& req);
	std::shared_ptr<BaseResponse> recieveFromServer(SOCKET& socket);
	std::shared_ptr<BaseResponse> createResponseFromCommand(short cmd);
};
