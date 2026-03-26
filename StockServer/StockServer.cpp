// StockServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <cstdint>
#include <charconv>
#include <optional>
#include <algorithm>

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


int8_t DESTROY_CONNECTION_METHOD = -1;

void run(int socketKey, std::shared_ptr<BaseRequest> req, NetworkManager& networkManager);
bool execute(int socketKey, NetworkManager& networkManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest> req);

void menus(int socketKey, NetworkManager& networkManager, ItemManager& itemManager);
void addItem(int socketKey, NetworkManager& networkManager, ItemManager& itemManager, std::shared_ptr<BaseRequest> req);
void removeItem(int socketKey, NetworkManager& networkManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest> req);
void printItemList(int socketKey, NetworkManager& networkManager, ItemManager& itemManager);
void addStock(int socketKey, NetworkManager& networkManager, ItemManager& itemManager, StockManager& stockManager, std::shared_ptr<BaseRequest> req);
void reduceStock(int socketKey, NetworkManager& networkManager, StockManager& stockManager, std::shared_ptr<BaseRequest> req);
void printItemType(int socketKey, NetworkManager& networkManager);


int main()
{
	MiddleManager middleManager;
	std::thread middleManagerT(&MiddleManager::loop, &middleManager);

	NetworkManager networkManager;
	// TODO: networkManager에서는 middleManager를 몰랐으면 좋겠는데
	std::thread networkManagerListenT(&NetworkManager::listenRequest, &networkManager, run);
	std::thread networkManagerResponseT(&NetworkManager::loop, &networkManager);

	while (true) {}

	networkManager.quit();
	networkManagerListenT.join();
	networkManagerResponseT.join();

	middleManager.quit();
	middleManagerT.join(); // middleManagerT 스레드가 끝나기를 기다림.

    return 0;
}

void run(int socketKey, std::shared_ptr<BaseRequest> req, NetworkManager& networkManager) {
	ItemManager itemManager;
	StockManager stockManager;

	// TODO: socketKey, req 를 middleManager에 전달.

	bool exeResult = execute(socketKey, networkManager, itemManager, stockManager, req);
		
	if (exeResult == false)
	{
		// TODO: 실패 시 서버에서 결과 반환 처리.
	}
}

bool execute(
	int socketKey,
	NetworkManager& networkManager, 
	ItemManager& itemManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest> req)
{

	switch (req->getCommand())
	{
	case Request::Command::ADD_ITEM:
		addItem(socketKey, networkManager, itemManager, req);
		return true;
	case Request::Command::REMOVE_ITEM:
		removeItem(socketKey, networkManager, itemManager, stockManager, req);
		return true;
	case Request::Command::PRINT_ITEM:
		printItemList(socketKey, networkManager, itemManager);
		return true;
	case Request::Command::ADD_STOCK:
		addStock(socketKey, networkManager, itemManager, stockManager, req);
		return true;
	case Request::Command::REDUCE_STOCK:
		reduceStock(socketKey, networkManager, stockManager, req);
		return true;
	case Request::Command::GET_ITEM_TYPE:
		printItemType(socketKey, networkManager);
		return true;
	case Request::Command::GET_MENU:
		menus(socketKey, networkManager, itemManager);
		return true;
	default:
		return false;
	}
}

// TODO: 서버에서 제공하는 API 목록으로 개선 필요.
void menus(
	int socketKey,
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
	std::string datas;
	int len = std::size(menuStr);
	for (int i = 0; i < len; i++)
	{
		datas += menuStr[i];
		if (i != len - 1) datas += ',';
	}

	std::string msg = "success";
	std::shared_ptr<BaseResponse> res = std::make_shared<GetMenusResponse>(true, msg, datas);
	// TODO: res nullptr 처리.
	networkManager.sendToClient(socketKey, res);
}

void printItemType(int socketKey, NetworkManager& networkManager)
{
	std::vector<std::string> itemTypeStr = ItemTypeHelper::getAllItemInfosToString();
	std::string msg = "success";
	
	std::string datas;
	int len = static_cast<int>(itemTypeStr.size());
	for (int i = 0; i < len; i++)
	{
		datas += itemTypeStr[i];
		if (i != len - 1) datas += ',';
	}

	std::shared_ptr<BaseResponse> res = std::make_shared<GetItemTypesResponse>(true, msg, datas);
	// TODO: res nullptr 처리.
	networkManager.sendToClient(socketKey, res);
}

void addItem(
	int socketKey,
	NetworkManager& networkManager,
	ItemManager& itemManager, 
	std::shared_ptr<BaseRequest> req
) {
	auto paredReq = std::dynamic_pointer_cast<AddItemRequest>(req);
	std::shared_ptr<BaseResponse> res = nullptr;

	if (itemManager.addItem(
		paredReq.get() ->getName(),
			static_cast<ItemType>(paredReq.get()->getItemType() - 1)
			) == StockServer::StatusCode::OK
		) {
		std::string msg = "아이템이 추가되었습니다.\n";
		res = std::make_shared<AddItemResponse>(true, msg);
	}
	else {
		std::string msg = "아이템 추가에 실패했습니다.\n";
		res = std::make_shared<AddItemResponse>(false, msg);
	}
	
	// TODO: res nullptr 처리.
	networkManager.sendToClient(socketKey, res);
}

void removeItem(
	int socketKey,
	NetworkManager& networkManager,
	ItemManager& itemManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest> req
) {
	auto paredReq = std::dynamic_pointer_cast<RemoveItemRequest>(req);
	std::shared_ptr<BaseResponse> res = nullptr;

	std::string msg;
	std::shared_ptr<Stock> stock = stockManager.findStockByItemId(paredReq.get()->getItemId());
	if (stock != nullptr)
	{
		msg = "해당 아이템은 재고가 남아있어 삭제할 수 없습니다.\n";
		res = std::make_shared<RemoveItemResponse>(false, msg);
		// TODO: res nullptr 처리.

		networkManager.sendToClient(socketKey, res);
		return;
	}

	if (itemManager.removeItem(
		paredReq.get()->getItemId()
	) == StockServer::StatusCode::OK)
	{
		msg = std::to_string(paredReq.get()->getItemId()) + " 아이템이 삭제되었습니다.\n";
		res = std::make_shared<RemoveItemResponse>(true, msg);
	}
	else
	{
		msg = "아이템을 삭제할 수 없습니다.\n";
		res = std::make_shared<RemoveItemResponse>(false, msg);
	}

	// TODO: res nullptr 처리.
	networkManager.sendToClient(socketKey, res);
}

void printItemList(
	int socketKey,
	NetworkManager& networkManager, 
	ItemManager& itemManager
) {
	std::string itemListStr = itemManager.itemListToString();
	std::string msg = "success";
	
	std::shared_ptr<BaseResponse> res = std::make_shared<PrintItemResponse>(true, msg, itemListStr);
	// TODO: res nullptr 처리.
	networkManager.sendToClient(socketKey, res);
}

void addStock(
	int socketKey,
	NetworkManager& networkManager,
	ItemManager& itemManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest> req
) {
	auto paredReq = std::dynamic_pointer_cast<AddStockRequest>(req);
	std::shared_ptr<BaseResponse> res = nullptr;

	std::string msg;

	std::shared_ptr<Item> item = itemManager.findItemById(paredReq.get()->getItemId());
	if (item == nullptr)
	{
		msg = "아이템이 존재하지 않습니다\n";
		res = std::make_shared<AddStockResponse>(false, msg);

		networkManager.sendToClient(socketKey, res);
		return;
	}

	if (stockManager.addStock(
		paredReq.get()->getItemId(), 
		paredReq.get()->getCount()
	) == StockServer::StatusCode::OK)
	{
		msg = "재고가 늘어났습니다.\n";
		res = std::make_shared<AddStockResponse>(true, msg);
	}
	else
	{
		msg = "재고를 추가할 수 없습니다.\n";
		res = std::make_shared<AddStockResponse>(false, msg);
	}

	// TODO: res nullptr 처리.
	networkManager.sendToClient(socketKey, res);
}

void reduceStock(
	int socketKey,
	NetworkManager& networkManager, 
	StockManager& stockManager, 
	std::shared_ptr<BaseRequest> req
) {
	auto paredReq = std::dynamic_pointer_cast<ReduceStockRequest>(req);
	std::shared_ptr<BaseResponse> res = nullptr;

	std::string msg;

	if (stockManager.reduceStock(
		paredReq.get()->getItemId(), 
		paredReq.get()->getCount()
	) == StockServer::StatusCode::OK)
	{
		msg = "재고가 삭제되었습니다.\n";
		res = std::make_shared<ReduceStockResponse>(true, msg);
	}
	else
	{
		msg = "재고 삭제에 실패했습니다.\n";
		res = std::make_shared<ReduceStockResponse>(false , msg);
	}

	// TODO: res nullptr 처리.
	networkManager.sendToClient(socketKey, res);
}

// TODO
//void getStockList()
//{
//	if (shared_ptr<Stock> stock = stockManager.findStockByItemId(itemId)) {
//		std::string data = std::to_string(stock->getItemId())
//			+ "재고 수 : "
//			+ std::to_string(stock->getCount())
//			+ "\n";
//		printFromSocket(socketKey, 1, msg, data);
//	}
//	else {
//		msg += "재고 조회에 실패했습니다. 다시 시도해주세요.\n";
//		printFromSocket(socketKey, 0, msg);
//	}
//}
