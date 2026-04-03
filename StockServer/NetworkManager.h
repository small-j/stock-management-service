#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "common.h"

class App;
class BaseRequest;
class BaseResponse;

struct ClientSocketInfo {
	SOCKET clientSocket;
	SOCKADDR_IN acceptSocketAddr = {};
};

class NetworkManager {
public:
	NetworkManager(App* app);
	virtual ~NetworkManager() = default;

private:
	static constexpr int SOCK_CONNECTION_MAX = 10000;
	bool _isQuitRequested = false;
	App* _owner;

	static constexpr unsigned int PACKET_SIZE = 1024;
	static constexpr unsigned int PORT = 4578;

	std::mutex _jobQueueMutex;
	std::queue < std::pair<int, std::shared_ptr<BaseResponse> > > _jobQueue;

	std::map<int, ClientSocketInfo> _clientSocks;
	
	SOCKET initSocket(WSADATA& wsa);
	void clearSocket(int key);
	int publishSocketHandleKey();
	int addClientConnection(SOCKET&, SOCKADDR_IN&);
	void ErrorHandler(const std::string& errMsg);

public:
	bool isQuitRequested() {
		return _isQuitRequested;
	}
	void quit();

	void listenRequest();

	std::shared_ptr<BaseRequest> recieveFromClient(SOCKET& socket);

	StockServer::StatusCode loop(); // queue watching + pop job
	StockServer::StatusCode addResponse(int socketKey, std::shared_ptr<BaseResponse> req);
	StockServer::StatusCode popResponse();
	void sendToClient(int socketKey, std::shared_ptr<BaseResponse> res);
};
