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
#include "RemoveItemRequest.h"
#include "PrintItemRequest.h"
#include "GetItemTypesRequest.h"
#include "AddStockRequest.h"
#include "ReduceStockRequest.h"
#include "GetMenusRequest.h"


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

void NetworkManager::listenRequest(void (*run)(SOCKET, MiddleManager&, NetworkManager&), MiddleManager& middleManager) {
	WSADATA wsa;
	if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		ErrorHandler("원속을 초기화 할 수 없습니다.");

	SOCKET listenSocket = initSocket(wsa);

	// 연결 수신 대기
	//while (isQuitRequested() == false) { // true가 되면 accept를 강제로 멈출 수 있는 방법이 있을까?
		 // 소켓이 동시에 처리할 수 있는 최대 연결 수.
		// 연결 클라이언트 정보 수신할 구조체.
		SOCKADDR_IN acceptSocketAddr = {};
		int acceptSocketAddrSize = sizeof(acceptSocketAddr);

		SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&acceptSocketAddr, &acceptSocketAddrSize);
		std::cout << "클라이언트가 연결되었습니다.\n";
		

		//addClientConnection(clientSocket);
		
		// TODO: main의 run  메소드 실행.
		run(clientSocket, middleManager, std::ref(*this));
	//}


	closesocket(listenSocket);
	closesocket(clientSocket); // test
	WSACleanup();
}

void NetworkManager::clearSocket() {
	// TODO: map으로부터 client socket 찾아서 close.
	//closesocket(clientSocket);
}

void NetworkManager::ErrorHandler(const std::string& errMsg)
{
	std::cout << errMsg << std::endl;
	::WSACleanup();
	// TODO: 전체 client 소켓 닫아주기
	exit(1);
}

void NetworkManager::addClientConnection() {
	// TODO: client socket map 추가
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
			// sendToClient
			// clearSocket();

			// TODO: popRequest 실패했을 때 어떻게 할지?
			popResponse();
			clearSocket();
		}
	}

	// TODO: 모든 clientSocket 정리하는 코드 작성.

	return StockServer::StatusCode::OK;
}

StockServer::StatusCode NetworkManager::addResponse(std::shared_ptr<BaseRequest> req) {
	if (!req) return StockServer::StatusCode::CANCELLED;

	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	_jobQueue.push(req);
	return StockServer::StatusCode::OK;
}

StockServer::StatusCode NetworkManager::popResponse() {
	std::lock_guard<std::mutex> lock(_jobQueueMutex);

	_jobQueue.pop();

	return StockServer::StatusCode::OK;
}

// 나중에는 send, receive 나누고 queue에 각각 넣어줄 것이다.
void NetworkManager::sendToClient(SOCKET& socket, BaseResponse& res) {
	// TODO: 버퍼 용량 넘는 경우 처리 필요.
	//int maxDataSize = PACKET_SIZE - RES_STATUS_SIZE - RES_MESSAGE_SIZE;

	//if (data != std::nullopt 
	//	&& data.value().size() > maxDataSize) {
	//	throw std::out_of_range("데이터 크기가 버퍼 용량을 초과했습니다.");
	//}
	char sendBuffer[PACKET_SIZE];
	memset(sendBuffer, '\0', PACKET_SIZE);

	res.serialize(sendBuffer);
	send(socket, sendBuffer, PACKET_SIZE, 0);
}
