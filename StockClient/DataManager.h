#pragma once
#include <winsock.h>
#include <memory>

#define PACKET_SIZE 1024 // byte

class BaseRequest;
class BaseResponse;

class DataManager {
public:
	DataManager() = default;
	virtual ~DataManager() = default;

	void sendToServer(SOCKET& serverSocket, BaseRequest& req);
	std::shared_ptr<BaseResponse> recieveFromServer(SOCKET& socket);
	std::shared_ptr<BaseResponse> createResponseFromCommand(short cmd);
};
