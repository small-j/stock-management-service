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

#include <thread>

#include "ItemTypeHelper.h"
#include "ItemManager.h"
#include "StockManager.h"
#include "Item.h"
#include "Stock.h"
#include "NetworkManager.h"
#include "MiddleManager.h"
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

int8_t DESTROY_CONNECTION_METHOD = -1;

void run(SOCKET clientSocket, MiddleManager& middleManager, NetworkManager& networkManager);
bool execute(SOCKET& clientSocket, NetworkManager& networkManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest>& req);

void menus(SOCKET& clientSocket, NetworkManager& networkManager, ItemManager& itemManager);
void addItem(SOCKET& clientSocket, NetworkManager& networkManager, ItemManager& itemManager, std::shared_ptr<BaseRequest>& req);
void removeItem(SOCKET& clientSocket, NetworkManager& networkManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest>& req);
void printItemList(SOCKET& clientSocket, NetworkManager& networkManager, ItemManager& itemManager);
void addStock(SOCKET& clientSocket, NetworkManager& networkManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest>& req);
void reduceStock(SOCKET& clientSocket, NetworkManager& networkManager, StockManager& stockManager, std::shared_ptr<BaseRequest>& req);
void printItemType(SOCKET& clientSocket, NetworkManager& networkManager);


int main()
{
	MiddleManager middleManager;
	std::thread middleManagerT(&MiddleManager::loop, &middleManager);

	NetworkManager networkManager;
	// TODO: networkManager에서는 middleManager를 몰랐으면 좋겠는데
	std::thread networkManagerListenT(&NetworkManager::listenRequest, &networkManager, run, std::ref(middleManager)); // TODO: client 요청 + socket key를 middle manager에 추가하는 main의 함수 넘겨주기.
	std::thread networkManagerResponseT(&NetworkManager::loop, &networkManager);

	networkManager.quit();
	networkManagerListenT.join();
	networkManagerResponseT.join();

	middleManager.quit();
	middleManagerT.join(); // middleManagerT 스레드가 끝나기를 기다림.

    return 0;
}

void run(SOCKET clientSocket, MiddleManager& middleManager, NetworkManager& networkManager) {
	ItemManager itemManager;
	StockManager stockManager;

	while (true)
	{
		std::shared_ptr<BaseRequest> req = networkManager.recieveFromClient(clientSocket);
		if (req == nullptr) {
			// log 처리
			break; 
		}

		// TODO 추가 실패하면 3번 재시도. 3번 실패하면 유저에게 실패 메시지 반환하기.
		middleManager.addRequest(req); 
		bool exeResult = execute(clientSocket, networkManager, itemManager, stockManager, req);
		
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
	NetworkManager& networkManager, 
	ItemManager& itemManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest>&req)
{

	switch (req.get()->getCommand())
	{
	case Request::Command::ADD_ITEM:
		addItem(clientSocket, networkManager,  itemManager, req);
		return true;
	case Request::Command::REMOVE_ITEM:
		removeItem(clientSocket, networkManager, itemManager, stockManager, req);
		return true;
	case Request::Command::PRINT_ITEM:
		printItemList(clientSocket, networkManager, itemManager);
		return true;
	case Request::Command::ADD_STOCK:
		addStock(clientSocket, networkManager, itemManager, stockManager, req);
		return true;
	case Request::Command::REDUCE_STOCK:
		reduceStock(clientSocket, networkManager, stockManager, req);
		return true;
	case Request::Command::GET_ITEM_TYPE:
		printItemType(clientSocket, networkManager);
		return true;
	case Request::Command::GET_MENU:
		menus(clientSocket, networkManager, itemManager);
		return true;
	default:
		return false;
	}
}

// TODO: 서버에서 제공하는 API 목록으로 개선 필요.
void menus(
	SOCKET& clientSocket, 
	NetworkManager& networkManager, 
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
	networkManager.sendToClient(clientSocket, res);
}

void printItemType(SOCKET& clientSocket, NetworkManager& networkManager)
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
	networkManager.sendToClient(clientSocket, res);
}

void addItem(
	SOCKET& clientSocket, 
	NetworkManager& networkManager,
	ItemManager& itemManager, 
	std::shared_ptr<BaseRequest>& req
) {
	auto paredReq = std::dynamic_pointer_cast<AddItemRequest>(req);
	AddItemResponse res;

	if (itemManager.addItem(
		paredReq.get() ->getName(),
			static_cast<ItemType>(paredReq.get()->getItemType() - 1)
			) == StockServer::StatusCode::OK
		) {
		std::string msg = "아이템이 추가되었습니다.\n";
		res = AddItemResponse(true, msg);
	}
	else {
		std::string msg = "아이템 추가에 실패했습니다.\n";
		res = AddItemResponse(true, msg);
	}

	networkManager.sendToClient(clientSocket, res);
}

void removeItem(
	SOCKET& clientSocket, 
	NetworkManager& networkManager,
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
		networkManager.sendToClient(clientSocket, res);
		return;
	}

	if (itemManager.removeItem(
		paredReq.get()->getItemId()
	) == StockServer::StatusCode::OK)
	{
		msg = std::to_string(paredReq.get()->getItemId()) + " 아이템이 삭제되었습니다.\n";
		res = RemoveItemResponse(true, msg);
	}
	else
	{
		msg = "아이템을 삭제할 수 없습니다.\n";
		res = RemoveItemResponse(true, msg);
	}

	networkManager.sendToClient(clientSocket, res);
}

void printItemList(
	SOCKET& clientSocket, 
	NetworkManager& networkManager, 
	ItemManager& itemManager
) {
	std::string itemListStr = itemManager.itemListToString();
	std::string msg = "success";

	PrintItemResponse res(true, msg, itemListStr);
	networkManager.sendToClient(clientSocket, res);
}

void addStock(
	SOCKET& clientSocket, 
	NetworkManager& networkManager,
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
		networkManager.sendToClient(clientSocket, res);
		return;
	}

	if (stockManager.addStock(
		paredReq.get()->getItemId(), 
		paredReq.get()->getCount()
	) == StockServer::StatusCode::OK)
	{
		msg = "재고가 늘어났습니다.\n";
		res = AddStockResponse(true, msg);
	}
	else
	{
		msg = "재고를 추가할 수 없습니다.\n";
		res = AddStockResponse(true, msg);
	}

	networkManager.sendToClient(clientSocket, res);
}

void reduceStock(
	SOCKET& clientSocket,
	NetworkManager& networkManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest>& req
) {
	auto paredReq = std::dynamic_pointer_cast<ReduceStockRequest>(req);
	ReduceStockResponse res;

	std::string msg;

	if (stockManager.reduceStock(
		paredReq.get()->getItemId(), 
		paredReq.get()->getCount()
	) == StockServer::StatusCode::OK)
	{
		msg = "재고가 삭제되었습니다.\n";
		res = ReduceStockResponse(true, msg);
	}
	else
	{
		msg = "재고 삭제에 실패했습니다.\n";
		res = ReduceStockResponse(true, msg);
	}

	networkManager.sendToClient(clientSocket, res);
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
