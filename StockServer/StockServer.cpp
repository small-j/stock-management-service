// StockServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <cstdint>
#include <charconv>
#include <optional>
#include <algorithm>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "ItemTypeHelper.h"
#include "ItemManager.h"
#include "StockManager.h"
#include "Item.h"
#include "Stock.h" // Stock의 정의가 필요하므로 추가

using namespace std;

#define PORT 4578
#define PACKET_SIZE 1024

#define REQ_COMMAND_SIZE 2
#define RES_STATUS_SIZE 2
#define RES_MESSAGE_SIZE 90

int8_t DESTROY_CONNECTION_METHOD = -1;
int8_t PRINT_METHOD = 1;
int8_t INPUT_METHOD = 2;
int8_t PRINT_AND_INPUT_METHOD = 3;

void ErrorHandler(const string& errMsg);
void run(SOCKET& clientSocket);
void printFromSocket(SOCKET& clientSocket, short status, string& msg, std::optional<std::string_view> data);

bool execute(SOCKET& clientSocket, short command, ItemManager& itemManager, StockManager& stockManager, const char* dataPtr);

void menus(SOCKET& clientSocket, ItemManager& itemManager);
void addItem(SOCKET& clientSocket, ItemManager& itemManager, const char* dataPtr);
void removeItem(SOCKET& clientSocket, ItemManager& itemManager, StockManager& stockManager, const char* dataPtr);
void printItemList(SOCKET& clientSocket, ItemManager& itemManager);
void addStock(SOCKET& clientSocket, ItemManager& itemManager, StockManager& stockManager, const char* dataPtr);
void reduceStock(SOCKET& clientSocket, StockManager& stockManager, const char* dataPtr);
void printItemType(SOCKET& clientSocket);


int main()
{
    WSADATA wsa;
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        ErrorHandler("원속을 초기화 할 수 없습니다.");

    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        ErrorHandler("접속 대기 소켓을 생성할 수 없습니다.");

    SOCKADDR_IN listenSocketAddr = { 0 };
    listenSocketAddr.sin_family = AF_INET;
    listenSocketAddr.sin_port = htons(PORT); // 요청을 수신할 소켓 프로세스가 사용할 포트번호
    listenSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 어떤 주소든 받는다.
    

    // 아래부터 예외처리 안해도 되나?

    // 소켓을 특정 주소 및 포트와 연결
    bind(listenSocket, (SOCKADDR*)&listenSocketAddr, sizeof(listenSocketAddr));
    // 연결 수신 대기 함수:listen
    listen(listenSocket, SOMAXCONN); // 소켓이 동시에 처리할 수 있는 최대 연결 수

    // 연결 클라이언트 정보 수신할 구조체
    SOCKADDR_IN acceptSocketAddr = {};
    int acceptSocketAddrSize = sizeof(acceptSocketAddr);
    // accept : 요청 연결 수락
    // 연결 수락되면서 연결된 클라이언트의 주소 정보도 기록됨.
    // 두번째 인수가 가리키는 버퍼의 크기를 지정?
    // accept는 들어오는 연결을 처리하는 소켓의 핸들을 반환한다. (새로 만들어진 소켓)
    // accept 함수가 실행되는 순간 프로그램의 실행이 그 지점에서 멈춤.
	// 대기열(Backlog)에 연결 요청이 들어올 때까지 CPU 자원을 거의 사용하지 않으면서 잠을 자는 상태가 됨.
	// 클라이언트가 connect를 호출하면(이 프로그램의 프로세스가 연결된 포트로) 
	// 운영체제가 이 연결 요청을 수락하고, accept 함수가 반환됨.

	SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&acceptSocketAddr, &acceptSocketAddrSize);
	cout << "클라이언트가 연결되었습니다.\n";

	run(clientSocket);

    closesocket(clientSocket);
    closesocket(listenSocket);

    WSACleanup();
    return 0;
}

void ErrorHandler(const string& errMsg)
{
	cout << errMsg << endl;
	::WSACleanup();
	exit(1);
}

void run(SOCKET& clientSocket) {
	char buffer[PACKET_SIZE] = {};

	ItemManager itemManager;
	StockManager stockManager;

	while (INT32 res = recv(clientSocket, buffer, PACKET_SIZE, 0))
	{
		if (res == -1) break;

		short command;
		memcpy(&command, buffer, sizeof(REQ_COMMAND_SIZE));
		std::string data(buffer + REQ_COMMAND_SIZE);
		const char* dataPtr = buffer + REQ_COMMAND_SIZE;

		if (execute(clientSocket, command, itemManager, stockManager, dataPtr) == false)
		{
			char response[PACKET_SIZE];
			memset(response, '\0', PACKET_SIZE);
			memcpy(response, &DESTROY_CONNECTION_METHOD, sizeof(DESTROY_CONNECTION_METHOD));

			send(clientSocket, response, sizeof(response), 0);
			break;
		}
	}
}

void printFromSocket(SOCKET& clientSocket, short status, std::string& msg, std::optional<std::string_view> data = std::nullopt)
{
	//int maxDataSize = PACKET_SIZE - RES_STATUS_SIZE - RES_MESSAGE_SIZE;

	//if (data != std::nullopt 
	//	&& data.value().size() > maxDataSize) {
	//	throw std::out_of_range("데이터 크기가 버퍼 용량을 초과했습니다."); // 질문
	//}

	int offset = 0;
	char sendBuffer[PACKET_SIZE];
	memset(sendBuffer, '\0', PACKET_SIZE);

	memcpy(sendBuffer, &status, RES_STATUS_SIZE);
	offset += RES_STATUS_SIZE;

	memcpy(sendBuffer + offset, msg.c_str(), msg.length());
	offset += RES_MESSAGE_SIZE;

	if (data.has_value()) {
		int dataSize = static_cast<int>(data.value().size());

		memcpy(sendBuffer + offset, &dataSize, sizeof(dataSize));
		offset += sizeof(dataSize);

		memcpy(sendBuffer + offset, data.value().data(), dataSize);
		offset += dataSize;
	}
	
	send(clientSocket, sendBuffer, PACKET_SIZE, 0);
}

bool execute(SOCKET& clientSocket, short command, ItemManager& itemManager, StockManager& stockManager, const char* dataPtr)
{
	switch (command)
	{
	case 0:
		addItem(clientSocket, itemManager, dataPtr);
		return true;
	case 1:
		removeItem(clientSocket, itemManager, stockManager, dataPtr);
		return true;
	case 2:
		printItemList(clientSocket, itemManager);
		return true;
	case 3:
		addStock(clientSocket, itemManager, stockManager, dataPtr);
		return true;
	case 4:
		reduceStock(clientSocket, stockManager, dataPtr);
		return true;
	case 5:
		printItemType(clientSocket);
		return true;
	case 6:
		menus(clientSocket, itemManager);
		return true;
	default:
		return false;
	}
}

void menus(SOCKET& clientSocket, ItemManager& itemManager)
{
	std::string menuStr[] = {
		"1. 아이템 추가", 
		"2. 아이템 삭제",
		"3. 아이템 목록 조회",
		"4. 재고 추가",
		"5. 재고 삭제",
		"그 외. 종료"
	};

	std::string msg = "success";
	string datas;
	int len = std::size(menuStr);
	for (int i = 0; i < len; i++)
	{
		datas += menuStr[i];
		if (i != len - 1) datas += ',';
	}

	printFromSocket(clientSocket, 1, msg, datas);
}

void printItemType(SOCKET& clientSocket)
{
	std::vector<std::string> itemTypeStr = ItemTypeHelper::getAllItemInfosToString();
	std::string msg = "success";
	
	string datas;
	int len = itemTypeStr.size();
	for (int i = 0; i < len; i++)
	{
		datas += itemTypeStr[i];
		if (i != len - 1) datas += ',';
	}
	printFromSocket(clientSocket, 1, msg, datas);
}

void addItem(SOCKET& clientSocket, ItemManager& itemManager, const char* dataPtr)
{
	int offset = 0;

	int nameLen = 0;
	memcpy(&nameLen, dataPtr + offset, sizeof(int));
	offset += sizeof(nameLen);

	string name(dataPtr + offset, nameLen);
	offset += nameLen;

	int itemType = -1; // 4byte
	memcpy(&itemType, dataPtr + offset, sizeof(itemType));

	if (itemManager.addItem(name, static_cast<ItemType>(itemType - 1)))
	{
		std::string msg = "아이템이 추가되었습니다.\n";
		printFromSocket(clientSocket, 1, msg);
		return;
	}

	std::string msg = "아이템 추가에 실패했습니다.\n";
	printFromSocket(clientSocket, 0, msg);
}

void removeItem(SOCKET& clientSocket, ItemManager& itemManager, StockManager& stockManager, const char* dataPtr)
{
	unsigned int itemId = Item::INVALID_ID;
	memcpy(&itemId, dataPtr, sizeof(itemId));
	
	std::string msg;
	shared_ptr<Stock> stock = stockManager.findStockByItemId(itemId);
	if (stock != nullptr)
	{
		msg = "해당 아이템은 재고가 남아있어 삭제할 수 없습니다.\n";
		printFromSocket(clientSocket, 0, msg);
		return;
	}

	if (itemManager.removeItem(itemId))
	{
		msg = std::to_string(itemId) + "아이템이 삭제되었습니다.\n";
		printFromSocket(clientSocket, 1, msg);
	}
	else
	{
		msg = "아이템을 삭제할 수 없습니다.\n";
		printFromSocket(clientSocket, 0, msg);
	}
}

void printItemList(SOCKET& clientSocket, ItemManager& itemManager)
{
	std::string itemListStr = itemManager.itemListToString();
	std::string msg = "success";
	printFromSocket(clientSocket, 1, msg, itemListStr);
}

void addStock(SOCKET& clientSocket, ItemManager& itemManager, StockManager& stockManager, const char* dataPtr)
{
	unsigned int itemId;
	unsigned int count;

	memcpy(&itemId, dataPtr, sizeof(itemId));
	memcpy(&count, dataPtr + sizeof(itemId), sizeof(count));

	std::string msg;

	shared_ptr<Item> item = itemManager.findItemById(itemId);
	if (item == nullptr)
	{
		msg = "아이템이 존재하지 않습니다\n";
		printFromSocket(clientSocket, 0, msg);
		return;
	}

	if (stockManager.addStock(itemId, count))
	{
		msg = "재고가 늘어났습니다.\n";
		printFromSocket(clientSocket, 0, msg);
	}
	else
	{
		msg = "재고를 추가할 수 없습니다.\n";
		printFromSocket(clientSocket, 0, msg);
	}
}

void reduceStock(SOCKET& clientSocket, StockManager& stockManager, const char* dataPtr)
{
	unsigned int itemId;
	unsigned int count;

	memcpy(&itemId, dataPtr, sizeof(itemId));
	memcpy(&count, dataPtr + sizeof(itemId), sizeof(count));

	std::string msg;

	if (stockManager.reduceStock(itemId, count))
	{
		msg = "재고가 삭제되었습니다.\n";
		printFromSocket(clientSocket, 1, msg);
	}
	else
	{
		msg = "재고 삭제에 실패했습니다.\n";
		printFromSocket(clientSocket, 0, msg);
	}
}

// TODO
//void getStockList()
//{
//	if (shared_ptr<Stock> stock = stockManager.findStockByItemId(itemId)) {
//		std::string data = std::to_string(stock->getItemId())
//			+ "재고 수 : "
//			+ std::to_string(stock->getCount())
//			+ "\n";
//		printFromSocket(clientSocket, 1, msg, data);
//	}
//	else {
//		msg += "재고 조회에 실패했습니다. 다시 시도해주세요.\n";
//		printFromSocket(clientSocket, 0, msg);
//	}
//}
