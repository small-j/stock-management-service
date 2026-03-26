#include "pch.h"
#include <iostream>
#include <cassert>
#include <charconv>
#include <thread>
#include <chrono>

#include "NetworkManager.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include "RequestCommand.h"

#include "MiddleManager.h"

#include "AddItemRequest.h"
#include "AddItemResponse.h"
#include "RemoveItemRequest.h"
#include "RemoveItemResponse.h"
#include "PrintItemRequest.h"
#include "PrintItemResponse.h"
#include "GetItemTypesRequest.h"
#include "GetItemTypesResponse.h"
#include "AddStockRequest.h"
#include "AddStockResponse.h"
#include "ReduceStockRequest.h"
#include "ReduceStockResponse.h"
#include "GetMenusRequest.h"
#include "GetMenusResponse.h"

void NetworkManager::quit() {
	_isQuitRequested = true;
}

SOCKET NetworkManager::initSocket(WSADATA& wsa) {
	SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		ErrorHandler("접속 대기 소켓을 생성할 수 없습니다.");

	SOCKADDR_IN listenSocketAddr = { 0 };
	listenSocketAddr.sin_family = AF_INET;
	listenSocketAddr.sin_port = htons(PORT); // 요청을 수신할 소켓 프로세스가 사용할 포트번호.
	listenSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 어떤 주소든 받는다.

	// 소켓을 특정 주소 및 포트와 연결.
	bind(listenSocket, (SOCKADDR*)&listenSocketAddr, sizeof(listenSocketAddr));

	if (::listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		puts("ERROR: 리슨 상태로 전환할 수 없습니다.");
		return 0;
	}

	return listenSocket;
}

void NetworkManager::listenRequest(void (*run)(int, std::shared_ptr<BaseRequest>, NetworkManager&)) {
	WSADATA wsa;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		ErrorHandler("원속을 초기화 할 수 없습니다.");

	SOCKET listenSocket = initSocket(wsa);
	// 연결 클라이언트 정보 수신할 구조체.
	SOCKADDR_IN acceptSocketAddr = {};
	int acceptSocketAddrSize = sizeof(acceptSocketAddr);

	SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&acceptSocketAddr, &acceptSocketAddrSize);
	// TODO: 따로 스레드로 빼기.
	// true가 되면 accept를 강제로 멈출 수 있는 방법이 있을까?

	if (clientSocket == INVALID_SOCKET) {
		ErrorHandler("클라이언트 소켓 연결에 실패했습니다.");
	}
	std::cout << "클라이언트가 연결되었습니다.\n";
	
	int socketKey = addClientConnection(clientSocket, acceptSocketAddr);
	if (socketKey == -1) {
		std::cout << "test" << std::endl;
		// TODO: 유저에게 실패 response 반환.
		// sendToClient를 오버로드로 2개 만들어서 각각 socket일 때 socket을 찾아서 유저에게 response 할 때의 처리를 구분해줘야겠다.
		// 소켓 key를 발급받지 못한 경우 유저 요청을 처리하지 못한 거니까.
		// 일정 타임아웃 시간 끝나면 무조건 실패 response 보내는걸로 처리?
	}

	// 연결 수신 대기
	while (isQuitRequested() == false) {
		std::shared_ptr<BaseRequest> req = recieveFromClient(clientSocket);
		if (req == nullptr) {
			// log 처리
			// TODO: 유저에게 실패 response 반환. -> 요청 형식이 잘못되었습니다.
			clearSocket(socketKey);
			continue;
		}

		run(socketKey, req, std::ref(*this));
	}

	clearSocket(1); // TODO: 일단 현재는 싱글 클라이언트.
	closesocket(listenSocket);
	WSACleanup();
}

void NetworkManager::clearSocket(int key) {
	auto clientSockItr = _clientSocks.find(key);
	if (clientSockItr == _clientSocks.end()) {
		std::cout << "존재하지 않는 소켓입니다." << std::endl;
		return; 
		// clearSocket 이 실행된 후에 key는 무조건 사용할 수 없는 상태. 
		// 즉, socket이 삭제된 상태가 되니까 결과를 status로 반환할 필요는 없다.
	}

	// TODO: second 검사 해줘야함.
	closesocket(clientSockItr->second.clientSocket);
	_clientSocks.erase(clientSockItr);
}

void NetworkManager::ErrorHandler(const std::string& errMsg) {
	std::cout << errMsg << std::endl;
	::WSACleanup();
	// TODO: 전체 client 소켓 닫아주기
	quit();
}

int NetworkManager::publishSocketHandleKey() {
	for (int idx = 1; idx < NetworkManager::SOCK_CONNECTION_MAX; ++idx) {
		// TODO: 증가하다가 다시 돌아오는 형태로 바꿔주자.
		if (_clientSocks.find(idx) == _clientSocks.end())
			return idx;
	}

	return -1;
}

int NetworkManager::addClientConnection(SOCKET& clientSocket, SOCKADDR_IN& sockAddr) {
	// TODO: 여기 부분 mutex로 묶어야겠다.
	if (_clientSocks.size() < NetworkManager::SOCK_CONNECTION_MAX) {
		int sockKey = publishSocketHandleKey();
		if (sockKey != -1) {
			_clientSocks[sockKey] = { clientSocket, sockAddr };
			return sockKey;
		}
	}

	// TODO: 자리가 생길때까지 대기.
	// 만약 대기하다가 스레드가 끝나버리면 소켓이 close가 안되는 문제가 생겨버린다.

	return -1;
}

std::shared_ptr<BaseRequest> NetworkManager::recieveFromClient(SOCKET& socket) {
	char recvBuffer[PACKET_SIZE];
	memset(recvBuffer, '\0', PACKET_SIZE);

	INT32 result = recv(socket, recvBuffer, PACKET_SIZE, 0);

	if (result == -1 || result == 0) return nullptr;

	BaseRequest baseReq(Request::Command::UNKNOWN);
	baseReq.deserialize(recvBuffer);

	std::shared_ptr<BaseRequest> req = createRequestFromCommand(baseReq.getCommand());
	if (!req) {
		// TODO: log
		return nullptr;
	}

	if (req->deserialize(recvBuffer) == 0) {
		return nullptr;
	}

	return req;
}

std::shared_ptr<BaseRequest> NetworkManager::createRequestFromCommand(short cmd) {
	switch (cmd) {
	case Request::Command::ADD_ITEM:
		return std::make_shared<AddItemRequest>();
	case Request::Command::REMOVE_ITEM :
		return std::make_shared<RemoveItemRequest>();
	case Request::Command::GET_ITEM_TYPE:
		return std::make_shared<GetItemTypesRequest>();
	case Request::Command::PRINT_ITEM:
		return std::make_shared<PrintItemRequest>();
	case Request::Command::ADD_STOCK:
		return std::make_shared<AddStockRequest>();
	case Request::Command::REDUCE_STOCK:
		return std::make_shared<ReduceStockRequest>();
	case Request::Command::GET_MENU:
		return std::make_shared<GetMenusRequest>();
	default:
		assert(false);
		return nullptr;
	}
}

StockServer::StatusCode NetworkManager::loop() {
	while (isQuitRequested() == false) {
		if (_jobQueue.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); // cpu 점유 방지.
		}
		else {
			// execute(_jobQueue.front());
			sendToClient(_jobQueue.front().first, _jobQueue.front().second);
			clearSocket(_jobQueue.front().first);

			popResponse();
		}
	}

	// TODO: 모든 clientSocket 정리하는 코드 작성.

	return StockServer::StatusCode::OK;
}

StockServer::StatusCode NetworkManager::addResponse(int socketKey, std::shared_ptr<BaseResponse> res) {
	if (!res) return StockServer::StatusCode::CANCELLED;

	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	_jobQueue.push({ socketKey, res });
	return StockServer::StatusCode::OK;
}

StockServer::StatusCode NetworkManager::popResponse() {
	std::lock_guard<std::mutex> lock(_jobQueueMutex);

	_jobQueue.pop();

	return StockServer::StatusCode::OK;
}

void NetworkManager::sendToClient(int socketKey, std::shared_ptr<BaseResponse> res) {
	// TODO: 버퍼 용량 넘는 경우 처리 필요.
	//int maxDataSize = PACKET_SIZE - RES_STATUS_SIZE - RES_MESSAGE_SIZE;

	//if (data != std::nullopt 
	//	&& data.value().size() > maxDataSize) {
	//	throw std::out_of_range("데이터 크기가 버퍼 용량을 초과했습니다.");
	//}

	auto clientSockItr = _clientSocks.find(socketKey);
	if (clientSockItr == _clientSocks.end()) {
		std::cout << "존재하지 않는 소켓입니다." << std::endl;
		std::cout << "소켓이 존재하지 않아서 클라이언트에 요청을 반환할 수 없습니다.";
		return;
	}

	char sendBuffer[PACKET_SIZE];
	memset(sendBuffer, '\0', PACKET_SIZE);

	res->serialize(sendBuffer);
	send(clientSockItr->second.clientSocket, sendBuffer, PACKET_SIZE, 0);
}
