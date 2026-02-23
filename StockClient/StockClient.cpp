
#include <iostream>
#include <winsock.h>
#include <charconv>
#include <limits>
#include "DataManager.h"
#include "GetMenusRequest.h"
#include "GetMenusResponse.h"
#include "GetItemTypesRequest.h"
#include "GetItemTypesResponse.h"
#include "PrintItemRequest.h"
#include "PrintItemResponse.h"
#include "AddItemRequest.h"
#include "AddItemResponse.h"
#include "RemoveItemRequest.h"
#include "RemoveItemResponse.h"
#include "AddStockRequest.h"
#include "AddStockResponse.h"
#include "ReduceStockRequest.h"
#include "ReduceStockResponse.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 4578

#define SERVER_IP "127.0.0.1"

// method -> 1byte
// method -1 -> destroy connection
// method 1 -> print data
// 
// data -> 1023byte
// method에서 지정된 형태의 데이터가 들어가있다

bool execute(SOCKET& serverSocket, short command, DataManager dataManager);

void printMenu(SOCKET& serverSocket, DataManager dataManager);
void addItem(SOCKET& serverSocket, DataManager dataManager);
void removeItem(SOCKET& serverSocket);
void printItemList(SOCKET& serverSocket, DataManager dataManager);
void addStock(SOCKET& serverSocket);
void reduceStock(SOCKET& serverSocket);

bool isValidItemId(int itemId);
bool isValidStockCount(long long count);

int main()
{

	WSADATA  wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET hSocket;
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	connect(hSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	DataManager dataManager;
	// TODO: menu 한번만 가져와서 계속 출력해주도록 변경?
	
	while (1)
	{
		printMenu(hSocket, dataManager);

		short command;
		std::cin >> command;


		if (!execute(hSocket, command, dataManager)) break;
	}

	closesocket(hSocket);

	WSACleanup();
	return 0;
}

// menu목록을 받아서 어떻게 아래 메소드들을 연결할지 고민이다.
bool execute(SOCKET& serverSocket, short command, DataManager dataManager)
{
	switch (command)
	{
	case 1:
		addItem(serverSocket, dataManager);
		return true;
	case 2:
		removeItem(serverSocket);
		return true;
	case 3:
		printItemList(serverSocket, dataManager);
		return true;
	case 4:
		addStock(serverSocket);
		return true;
	case 5:
		reduceStock(serverSocket);
		return true;
	default:
		return false;
	}
}

void printMenu(SOCKET& serverSocket, DataManager dataManager)
{
	GetMenusRequest req;
	GetMenusResponse res;
	dataManager.sendToServer(serverSocket, req, res);

	if (res.getStatus() == 1)
		std::cout << res.toString();
	else
		std::cout << res.getMessage();
}

GetItemTypesResponse printItemTypes(SOCKET& serverSocket, DataManager dataManager)
{
	GetItemTypesRequest req;
	GetItemTypesResponse res;
	dataManager.sendToServer(serverSocket, req, res);
	return res;
}

void addItem(SOCKET& serverSocket, DataManager dataManager)
{
	GetItemTypesResponse itemTypesRes = printItemTypes(serverSocket, dataManager);
	if (itemTypesRes.getStatus() != 1) {
		std::cout << itemTypesRes.getMessage();
		return;
	}

	// 필요한 데이터 입력 받기
	std::string name;
	int itemType = 0;
	std::cout << "아이템의 이름을 입력해주세요.\t";
	std::cin >> name;
	std::cout << "아이템의 타입의 번호를 선택해주세요.\n";
	std::cout << itemTypesRes.toString();
	std::cin >> itemType;

	AddItemRequest req(name, itemType);
	AddItemResponse res;
	dataManager.sendToServer(serverSocket, req, res);
	std::cout << res.getMessage();
}

void removeItem(SOCKET& serverSocket, DataManager dataManager)
{
	int itemId;
	std::cout << "아이템의 아이디를 입력해주세요.\t";
	std::cin >> itemId;

	if (isValidItemId(itemId) == false) {
		std::cout << "아이템 아이디가 올바르지 않습니다.\n";
		return;
	}

	unsigned int castItemId = static_cast<unsigned int>(itemId);

	RemoveItemRequest req(castItemId);
	RemoveItemResponse res;
	dataManager.sendToServer(serverSocket, req, res);
	std::cout << res.getMessage();
}

void printItemList(SOCKET& serverSocket, DataManager dataManager)
{
	PrintItemRequest req;
	PrintItemResponse res;
	dataManager.sendToServer(serverSocket, req, res);
	
	if (res.getStatus() == 1)
		std::cout << res.getItemList();
	else
		std::cout << res.getMessage();
}

void addStock(SOCKET& serverSocket, DataManager dataManager)
{
	int itemId;
	std::cout << "재고를 추가할 아이템 id를 입력해주세요.\t";
	std::cin >> itemId;

	long long count;
	std::cout << "재고 수를 입력해주세요.\t";
	std::cin >> count;

	if (isValidItemId(itemId) == false) {
		std::cout << "아이템 아이디가 올바르지 않습니다.\n";
		return;
	}
	if (isValidStockCount(count) == false) {
		std::cout << "재고 수가 올바르지 않습니다.\n";
		return;
	}

	unsigned int castItemId = static_cast<unsigned int>(itemId);
	unsigned int castCount = static_cast<unsigned int>(count);

	AddStockRequest req(castItemId, castCount);
	AddStockResponse res;
	dataManager.sendToServer(serverSocket, req, res);
	std::cout << res.getMessage();
}

void reduceStock(SOCKET& serverSocket, DataManager dataManager)
{
	int itemId;
	std::cout << "재고를 줄일 아이템 id를 입력해주세요.\t";
	std::cin >> itemId;

	long long count;
	std::cout << "삭제할 재고 수를 입력해주세요.\t";
	std::cin >> count;

	if (isValidItemId(itemId) == false) {
		std::cout << "아이템 아이디가 올바르지 않습니다.\n";
		return;
	}
	if (isValidStockCount(count) == false) {
		std::cout << "재고 수가 올바르지 않습니다.\n";
		return;
	}

	unsigned int castItemId = static_cast<unsigned int>(itemId);
	unsigned int castCount = static_cast<unsigned int>(count);

	ReduceStockRequest req(castItemId, castCount);
	ReduceStockResponse res;
	dataManager.sendToServer(serverSocket, req, res);
	std::cout << res.getMessage();
}

bool isValidItemId(int itemId)
{
	return 0 < itemId && itemId < 10000;
}

bool isValidStockCount(long long count)
{
	return 0 < count && count < (std::numeric_limits<unsigned int>::max)();
}
