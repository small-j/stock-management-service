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
#include "Stock.h"
#include "DataManager.h"
#include "RequestCommand.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include "GetMenusResponse.h"
#include "GetItemTypesResponse.h"
#include "AddItemRequest.h"
#include "AddItemResponse.h"
#include "RemoveItemRequest.h"
#include "RemoveItemResponse.h"
#include "PrintItemResponse.h"
#include "AddStockRequest.h"
#include "AddStockResponse.h"
#include "ReduceStockRequest.h"
#include "ReduceStockResponse.h"

using namespace std;

#define PORT 4578

int8_t DESTROY_CONNECTION_METHOD = -1;

void ErrorHandler(const string& errMsg);
void run(SOCKET& clientSocket);
bool execute(SOCKET& clientSocket, DataManager& dataManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest>& req);

void menus(SOCKET& clientSocket, DataManager& dataManager, ItemManager& itemManager);
void addItem(SOCKET& clientSocket, DataManager& dataManager, ItemManager& itemManager, std::shared_ptr<BaseRequest>& req);
void removeItem(SOCKET& clientSocket, DataManager& dataManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest>& req);
void printItemList(SOCKET& clientSocket, DataManager& dataManager, ItemManager& itemManager);
void addStock(SOCKET& clientSocket, DataManager& dataManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest>& req);
void reduceStock(SOCKET& clientSocket, DataManager& dataManager, StockManager& stockManager, std::shared_ptr<BaseRequest>& req);
void printItemType(SOCKET& clientSocket, DataManager& dataManager);


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
    listenSocketAddr.sin_port = htons(PORT); // 요청을 수신할 소켓 프로세스가 사용할 포트번호.
    listenSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 어떤 주소든 받는다.
    
    // 소켓을 특정 주소 및 포트와 연결.
    bind(listenSocket, (SOCKADDR*)&listenSocketAddr, sizeof(listenSocketAddr));
    // 연결 수신 대기 함수:listen.
    listen(listenSocket, SOMAXCONN); // 소켓이 동시에 처리할 수 있는 최대 연결 수.

    // 연결 클라이언트 정보 수신할 구조체.
    SOCKADDR_IN acceptSocketAddr = {};
    int acceptSocketAddrSize = sizeof(acceptSocketAddr);


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
	DataManager dataManager;
	ItemManager itemManager;
	StockManager stockManager;

	while (true)
	{
		std::shared_ptr<BaseRequest> req = dataManager.recieveFromClient(clientSocket);
		if (req == nullptr) {
			// log 처리
			break;
		}

		bool exeResult = execute(clientSocket, dataManager, itemManager, stockManager, req);
		
		if (exeResult == false)
		{
			// TODO: 아래 방식 개선 필요.
			char response[PACKET_SIZE];
			memset(response, '\0', PACKET_SIZE);
			memcpy(response, &DESTROY_CONNECTION_METHOD, sizeof(DESTROY_CONNECTION_METHOD));

			send(clientSocket, response, sizeof(response), 0);
			break;
		}
	}
}

bool execute(
	SOCKET& clientSocket, 
	DataManager& dataManager, 
	ItemManager& itemManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest>&req)
{

	switch (req.get()->getCommand())
	{
	case Request::Command::ADD_ITEM:
		addItem(clientSocket, dataManager,  itemManager, req);
		return true;
	case Request::Command::REMOVE_ITEM:
		removeItem(clientSocket, dataManager, itemManager, stockManager, req);
		return true;
	case Request::Command::PRINT_ITEM:
		printItemList(clientSocket, dataManager, itemManager);
		return true;
	case Request::Command::ADD_STOCK:
		addStock(clientSocket, dataManager, itemManager, stockManager, req);
		return true;
	case Request::Command::REDUCE_STOCK:
		reduceStock(clientSocket, dataManager, stockManager, req);
		return true;
	case Request::Command::GET_ITEM_TYPE:
		printItemType(clientSocket, dataManager);
		return true;
	case Request::Command::GET_MENU:
		menus(clientSocket, dataManager, itemManager);
		return true;
	default:
		return false;
	}
}

// TODO: 서버에서 제공하는 API 목록으로 개선 필요.
void menus(
	SOCKET& clientSocket, 
	DataManager& dataManager, 
	ItemManager& itemManager
) {
	std::string menuStr[] = {
		"1. 아이템 추가", 
		"2. 아이템 삭제",
		"3. 아이템 목록 조회",
		"4. 재고 추가",
		"5. 재고 삭제",
		"그 외. 종료"
	};
	string datas;
	int len = std::size(menuStr);
	for (int i = 0; i < len; i++)
	{
		datas += menuStr[i];
		if (i != len - 1) datas += ',';
	}

	std::string msg = "success";
	GetMenusResponse res(true, msg, datas);
	dataManager.sendToClient(clientSocket, res);
}

void printItemType(SOCKET& clientSocket, DataManager& dataManager)
{
	std::vector<std::string> itemTypeStr = ItemTypeHelper::getAllItemInfosToString();
	std::string msg = "success";
	
	string datas;
	int len = static_cast<int>(itemTypeStr.size());
	for (int i = 0; i < len; i++)
	{
		datas += itemTypeStr[i];
		if (i != len - 1) datas += ',';
	}

	GetItemTypesResponse res(true, msg, datas);
	dataManager.sendToClient(clientSocket, res);
}

void addItem(
	SOCKET& clientSocket, 
	DataManager& dataManager,
	ItemManager& itemManager, 
	std::shared_ptr<BaseRequest>& req
) {
	auto paredReq = std::dynamic_pointer_cast<AddItemRequest>(req);
	AddItemResponse res;

	if (itemManager.addItem(
		paredReq.get() ->getName(),
			static_cast<ItemType>(paredReq.get()->getItemType() - 1)
			)
		) {
		std::string msg = "아이템이 추가되었습니다.\n";
		res = AddItemResponse(true, msg);
	}
	else {
		std::string msg = "아이템 추가에 실패했습니다.\n";
		res = AddItemResponse(true, msg);
	}

	dataManager.sendToClient(clientSocket, res);
}

void removeItem(
	SOCKET& clientSocket, 
	DataManager& dataManager,
	ItemManager& itemManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest>& req
) {
	auto paredReq = std::dynamic_pointer_cast<RemoveItemRequest>(req);
	RemoveItemResponse res;
	
	std::string msg;
	shared_ptr<Stock> stock = stockManager.findStockByItemId(paredReq.get()->getItemId());
	if (stock != nullptr)
	{
		msg = "해당 아이템은 재고가 남아있어 삭제할 수 없습니다.\n";
		res = RemoveItemResponse(true, msg);
		dataManager.sendToClient(clientSocket, res);
		return;
	}

	if (itemManager.removeItem(paredReq.get()->getItemId()))
	{
		msg = std::to_string(paredReq.get()->getItemId()) + " 아이템이 삭제되었습니다.\n";
		res = RemoveItemResponse(true, msg);
	}
	else
	{
		msg = "아이템을 삭제할 수 없습니다.\n";
		res = RemoveItemResponse(true, msg);
	}

	dataManager.sendToClient(clientSocket, res);
}

void printItemList(
	SOCKET& clientSocket, 
	DataManager& dataManager, 
	ItemManager& itemManager
) {
	std::string itemListStr = itemManager.itemListToString();
	std::string msg = "success";

	PrintItemResponse res(true, msg, itemListStr);
	dataManager.sendToClient(clientSocket, res);
}

void addStock(
	SOCKET& clientSocket, 
	DataManager& dataManager,
	ItemManager& itemManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest>& req
) {
	auto paredReq = std::dynamic_pointer_cast<AddStockRequest>(req);
	AddStockResponse res;

	std::string msg;

	shared_ptr<Item> item = itemManager.findItemById(paredReq.get()->getItemId());
	if (item == nullptr)
	{
		msg = "아이템이 존재하지 않습니다\n";
		res = AddStockResponse(true, msg);
		dataManager.sendToClient(clientSocket, res);
		return;
	}

	if (stockManager.addStock(paredReq.get()->getItemId(), paredReq.get()->getCount()))
	{
		msg = "재고가 늘어났습니다.\n";
		res = AddStockResponse(true, msg);
	}
	else
	{
		msg = "재고를 추가할 수 없습니다.\n";
		res = AddStockResponse(true, msg);
	}

	dataManager.sendToClient(clientSocket, res);
}

void reduceStock(
	SOCKET& clientSocket,
	DataManager& dataManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest>& req
) {
	auto paredReq = std::dynamic_pointer_cast<ReduceStockRequest>(req);
	ReduceStockResponse res;

	std::string msg;

	if (stockManager.reduceStock(paredReq.get()->getItemId(), paredReq.get()->getCount()))
	{
		msg = "재고가 삭제되었습니다.\n";
		res = ReduceStockResponse(true, msg);
	}
	else
	{
		msg = "재고 삭제에 실패했습니다.\n";
		res = ReduceStockResponse(true, msg);
	}

	dataManager.sendToClient(clientSocket, res);
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
