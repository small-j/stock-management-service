#pragma once
#include <winsock.h>
#include <mutex>
#include <queue>

#define PACKET_SIZE 1024 // byte

class BaseRequest;
class BaseResponse;
class MiddleManager;

#define PORT 4578

class NetworkManager {
public:
	NetworkManager() = default;
	virtual ~NetworkManager() = default;

private:
	bool _isQuitRequested = false;
	std::mutex _jobQueueMutex;
	std::queue<std::shared_ptr<BaseRequest> > _jobQueue;
	
	SOCKET initSocket(WSADATA& wsa);
	void clearSocket();
	void addClientConnection();
	void ErrorHandler(const std::string& errMsg);

public:
	bool isQuitRequested() {
		return _isQuitRequested;
	}
	void quit();

	void listenRequest(void (*run)(SOCKET, MiddleManager&, NetworkManager&), MiddleManager& middleManager);

	std::shared_ptr<BaseRequest> recieveFromClient(SOCKET& socket);
	std::shared_ptr<BaseRequest> createRequestFromCommand(short cmd);

	StockServer::StatusCode loop(); // queue watching + pop job
	StockServer::StatusCode addResponse(std::shared_ptr<BaseRequest> req);
	StockServer::StatusCode popResponse();
	void sendToClient(SOCKET& socket, BaseResponse& res);
};
