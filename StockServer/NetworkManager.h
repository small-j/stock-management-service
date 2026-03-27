#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#include <mutex>
#include <queue>
#include <map>

#define PACKET_SIZE 1024 // byte

class BaseRequest;
class BaseResponse;

#define PORT 4578

struct ClientSocketInfo {
	SOCKET clientSocket;
	SOCKADDR_IN acceptSocketAddr = {};
};

class NetworkManager {
public:
	NetworkManager() = default;
	virtual ~NetworkManager() = default;

private:
	static constexpr int SOCK_CONNECTION_MAX = 10000;
	bool _isQuitRequested = false;

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

	void listenRequest(void (*run)(int, std::shared_ptr<BaseRequest>, NetworkManager&));

	std::shared_ptr<BaseRequest> recieveFromClient(SOCKET& socket);
	std::shared_ptr<BaseRequest> createRequestFromCommand(short cmd);

	StockServer::StatusCode loop(); // queue watching + pop job
	StockServer::StatusCode addResponse(int socketKey, std::shared_ptr<BaseResponse> req);
	StockServer::StatusCode popResponse();
	void sendToClient(int socketKey, std::shared_ptr<BaseResponse> res);
};
