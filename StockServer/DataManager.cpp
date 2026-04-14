#include "pch.h"
#include "DataManager.h"

#include <thread>
#include <chrono>
#include <memory>

#include "App.h"

#include "ItemTypeHelper.h"
#include "ItemManager.h"
#include "StockManager.h"
#include "Item.h"
#include "Stock.h"

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

DataManager::DataManager(App* app) : _owner(app) {
	_itemManager = std::make_shared <ItemManager>();
	_stockManager = std::make_shared<StockManager>();
}

void DataManager::quit() {
	LoggerService::debug("quit dataManager.");
	_isQuitRequested = true;
}

StockServer::StatusCode DataManager::loop() {
	while (isQuitRequested() == false) {
		if (_jobQueue.empty()) {
			LoggerService::debug("data manager queue가 비어있습니다.");
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); // cpu 점유 방지.
		}
		else {
			LoggerService::debug("request를 처리합니다. -> client : " + _jobQueue.front().first);
			execute(_jobQueue.front().first, _jobQueue.front().second);

			LoggerService::debug("request 처리가 완료되었습니다. -> client : " + _jobQueue.front().first);
			popRequest();
		}
	}

	return StockServer::StatusCode::OK;
}

StockServer::StatusCode DataManager::execute(
	int socketKey, 
	std::shared_ptr<BaseRequest> req
) {
	if (!req) return StockServer::StatusCode::CANCELLED;

	std::shared_ptr<BaseResponse> res = callApi(socketKey, req);

	if (res == nullptr) {
		LoggerService::error("데이터 처리 중 알 수 없는 문제가 발생했습니다. -> client : " + socketKey);
		return StockServer::StatusCode::CANCELLED;
		// res가 nullptr일 경우 유저는 결과를 반환받을 수 없다.
	}
	_owner->addResponse(socketKey, res);

	return StockServer::StatusCode::OK;
}

StockServer::StatusCode DataManager::addRequest(int socketKey, std::shared_ptr<BaseRequest> req) {
	if (!req) {
		LoggerService::error("request를 추가할 수 없습니다. : " + socketKey);
		return StockServer::StatusCode::CANCELLED;
	}

	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	_jobQueue.push({ socketKey, req });
	return StockServer::StatusCode::OK;
}

StockServer::StatusCode DataManager::popRequest() {
	std::lock_guard<std::mutex> lock(_jobQueueMutex);

	_jobQueue.pop();

	return StockServer::StatusCode::OK;
}

std::shared_ptr<BaseResponse> DataManager::callApi(
	int socketKey,
	std::shared_ptr<BaseRequest> req
) {

	switch (req->getCommand())
	{
	case Request::Command::ADD_ITEM:
		return addItem(socketKey, req);
	case Request::Command::REMOVE_ITEM:
		return removeItem(socketKey, req);
	case Request::Command::PRINT_ITEM:
		return printItemList(socketKey);
	case Request::Command::ADD_STOCK:
		return addStock(socketKey, req);
	case Request::Command::REDUCE_STOCK:
		return reduceStock(socketKey, req);
	case Request::Command::GET_ITEM_TYPE:
		return printItemType(socketKey);
	case Request::Command::GET_MENU:
		return menus(socketKey);
	default:
		return nullptr;
	}
}

// TODO: 서버에서 제공하는 API 목록으로 개선 필요.
std::shared_ptr<BaseResponse> DataManager::menus(
	int socketKey
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

	if (res == nullptr) {
		LoggerService::error("get menu 처리 중 알 수 없는 문제가 발생했습니다. : " + socketKey);
		return std::make_shared<GetMenusResponse>(); // TODO: 강제로 이 로직을 타도록 테스트하기.
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::printItemType(int socketKey)
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
	
	if (res == nullptr) {
		LoggerService::error("print item types 처리 중 알 수 없는 문제가 발생했습니다. : " + socketKey);
		return std::make_shared<GetItemTypesResponse>();
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::addItem(
	int socketKey,
	std::shared_ptr<BaseRequest> req
) {
	auto paredReq = std::dynamic_pointer_cast<AddItemRequest>(req);
	//TODO: 유저 입력 형식 문제 처리하기.

	std::shared_ptr<BaseResponse> res = nullptr;

	if (_itemManager->addItem(
		paredReq.get()->getName(),
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

	if (res == nullptr) {
		std::string msg = "알 수 없는 에러가 발생했습니다.";
		LoggerService::error("add item 처리 중 알 수 없는 문제가 발생했습니다. : " + socketKey);
		return std::make_shared<AddItemResponse>(false, msg);
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::removeItem(
	int socketKey,
	std::shared_ptr<BaseRequest> req
) {
	auto paredReq = std::dynamic_pointer_cast<RemoveItemRequest>(req);
	std::shared_ptr<BaseResponse> res = nullptr;

	std::string msg;
	std::shared_ptr<Stock> stock = _stockManager->findStockByItemId(paredReq.get()->getItemId());
	if (stock != nullptr)
	{
		msg = "해당 아이템은 재고가 남아있어 삭제할 수 없습니다.\n";
		res = std::make_shared<RemoveItemResponse>(false, msg);

		if (res == nullptr) {
			msg = "알 수 없는 에러가 발생했습니다.";
			return std::make_shared<RemoveItemResponse>(false, msg);
		}
		
		return res;
	}

	if (_itemManager->removeItem(
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

	if (res == nullptr) {
		LoggerService::error("remove item 처리 중 알 수 없는 문제가 발생했습니다. : " + socketKey);
		msg = "알 수 없는 에러가 발생했습니다.";
		return std::make_shared<RemoveItemResponse>(false, msg);
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::printItemList(
	int socketKey
) {
	std::string itemListStr = _itemManager->itemListToString();
	std::string msg = "success";

	std::shared_ptr<BaseResponse> res = std::make_shared<PrintItemResponse>(true, msg, itemListStr);
	
	if (res == nullptr) {
		LoggerService::error("print item list 처리 중 알 수 없는 문제가 발생했습니다. : " + socketKey);
		return std::make_shared<PrintItemResponse>();
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::addStock(
	int socketKey,
	std::shared_ptr<BaseRequest> req
) {
	auto paredReq = std::dynamic_pointer_cast<AddStockRequest>(req);
	std::shared_ptr<BaseResponse> res = nullptr;

	std::string msg;

	std::shared_ptr<Item> item = _itemManager->findItemById(paredReq.get()->getItemId());
	if (item == nullptr)
	{
		msg = "아이템이 존재하지 않습니다\n";
		res = std::make_shared<AddStockResponse>(false, msg);

		if (res == nullptr) {
			LoggerService::error("add stock 처리 중 알 수 없는 문제가 발생했습니다. : " + socketKey);
			msg = "알 수 없는 에러가 발생했습니다.";
			return std::make_shared<AddStockResponse>(false, msg);
		}

		return res;
	}

	if (_stockManager->addStock(
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

	if (res == nullptr) {
		LoggerService::error("add stock 처리 중 알 수 없는 문제가 발생했습니다. : " + socketKey);
		msg = "알 수 없는 에러가 발생했습니다.";
		return std::make_shared<AddStockResponse>(false, msg);
	}
	
	return res;
}

std::shared_ptr<BaseResponse> DataManager::reduceStock(
	int socketKey,
	std::shared_ptr<BaseRequest> req
) {
	auto paredReq = std::dynamic_pointer_cast<ReduceStockRequest>(req);
	std::shared_ptr<BaseResponse> res = nullptr;

	std::string msg;

	if (_stockManager->reduceStock(
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
		res = std::make_shared<ReduceStockResponse>(false, msg);
	}

	if (res == nullptr) {
		LoggerService::error("reduce stock 처리 중 알 수 없는 문제가 발생했습니다. : " + socketKey);
		msg = "알 수 없는 에러가 발생했습니다.";
		return std::make_shared<ReduceStockResponse>(false, msg);
	}

	return res;
}
