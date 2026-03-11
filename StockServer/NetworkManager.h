#pragma once
#include <winsock.h>

#define PACKET_SIZE 1024 // byte

class BaseRequest;
class BaseResponse;

class NetworkManager {
public:
	NetworkManager() = default;
	virtual ~NetworkManager() = default;

	void sendToClient(SOCKET& socket, BaseResponse& res);
	std::shared_ptr<BaseRequest> recieveFromClient(SOCKET& socket);
	std::shared_ptr<BaseRequest> createRequestFromCommand(short cmd);
};
