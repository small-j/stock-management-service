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

#include "ResponseFactory.h"
#include "DtoFactory.h"

#include "AddItemDto.h"
#include "RemoveItemDto.h"
#include "AddStockDto.h"
#include "ReduceStockDto.h"

DataManager::DataManager(App* const app) : _owner(app) {
	_itemManager = std::make_shared<ItemManager>();
	_stockManager = std::make_shared<StockManager>();
}

void DataManager::quit() {
	LoggerService::debug("quit dataManager.");
	_isQuitRequested = true;
}

void DataManager::loop() {
	while (isQuitRequested() == false) {
		if (_jobQueue.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); // cpu 점유 방지.
		}
		else {
			LoggerService::debug("request를 처리합니다. -> client : " + std::to_string(_jobQueue.front().socketKey));

			if (execute(_jobQueue.front()) == StockServer::StatusCode::OK) {
				LoggerService::debug("request 처리가 완료되었습니다. -> client : " + std::to_string(_jobQueue.front().socketKey));
			}

			popRequest();
		}
	}
}

StockServer::StatusCode DataManager::execute(RequestKeyPair keyP) {
	if (!keyP.req) return StockServer::StatusCode::CANCELLED;

	std::shared_ptr<BaseResponse> res = callApi(keyP);

	if (res == nullptr) {
		LoggerService::error("데이터 처리 중 문제가 발생했습니다. -> client : " + std::to_string(keyP.socketKey));
		return StockServer::StatusCode::CANCELLED;
		// TODO: res가 nullptr일 경우 status를 false 로 해서 기본 res 반환하도록 변경하자. res의 생성 책임이 dataManager에 있으니.
	}
	_owner->addResponse(ResponseKeyPair{ keyP.socketKey, res });

	return StockServer::StatusCode::OK;
}

StockServer::StatusCode DataManager::addRequest(RequestKeyPair keyP) {
	if (!keyP.req) {
		LoggerService::error("request 값이 정상적이지 않아 처리할 수 없습니다. : " + std::to_string(keyP.socketKey));
		return StockServer::StatusCode::CANCELLED;
	}

	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	_jobQueue.push({ keyP.socketKey, keyP. req });
	return StockServer::StatusCode::OK;
}

StockServer::StatusCode DataManager::popRequest() {
	std::lock_guard<std::mutex> lock(_jobQueueMutex);

	_jobQueue.pop();

	return StockServer::StatusCode::OK;
}

std::shared_ptr<BaseResponse> DataManager::callApi(RequestKeyPair keyP) {

	switch (keyP.req->getCommand())
	{
	case Request::Command::ADD_ITEM:
		return addItem(keyP);
	case Request::Command::REMOVE_ITEM:
		return removeItem(keyP);
	case Request::Command::PRINT_ITEM:
		return printItemList(keyP);
	case Request::Command::ADD_STOCK:
		return addStock(keyP);
	case Request::Command::REDUCE_STOCK:
		return reduceStock(keyP);
	case Request::Command::GET_ITEM_TYPE:
		return printItemType(keyP);
	case Request::Command::GET_MENU:
		return menus(keyP);
	default:
		return nullptr;
	}
}

// TODO: datas -> class DTO 형태로 바꿔서 클라이언트에 보내는 형태로 deserialize 하도록 하자. -> response에 class dto를 넘기면 알아서 deserialize 하는 형태로.
// DTO는 DataManager에서 알고 있는 형태고, 데이터를 클라이언트에 전달할 때 사용하는 공개된 데이터 형태이다.
// response는 dataManager와 분리.
// 문제는.. BaseDTO로 모든 DTO를 받도록 해야하나? -> factory class에서?

std::shared_ptr<BaseResponse> DataManager::menus(RequestKeyPair keyP) {
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
	std::shared_ptr<BaseResponse> res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), true, msg, datas);

	if (res == nullptr) {
		LoggerService::error("get menu 처리 중 알 수 없는 문제가 발생했습니다. : " + std::to_string(keyP.socketKey));
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand()); // TODO: 강제로 이 로직을 타도록 테스트하기.
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::printItemType(RequestKeyPair keyP)
{
	std::vector<std::string> itemTypeStr = ItemTypeHelper::getAllItemInfosToString();
	// TODO: 위의 경우에도 검사해야하나?  null이 들어갈 경우가 없는데.

	std::string msg = "success";

	std::string datas;
	int len = static_cast<int>(itemTypeStr.size());
	for (int i = 0; i < len; i++)
	{
		datas += itemTypeStr[i];
		if (i != len - 1) datas += ',';
	}

	std::shared_ptr<BaseResponse> res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), true, msg, datas);
	
	if (res == nullptr) {
		LoggerService::error("print item types 처리 중 알 수 없는 문제가 발생했습니다. : " + std::to_string(keyP.socketKey));
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand());
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::addItem(RequestKeyPair keyP) {
	std::shared_ptr<AddItemDto> dto = DtoFactory::getAddItemDtoFromReq(keyP.req);
	
	if (dto == nullptr) {
		std::string msg = "입력 형식이 잘못되었습니다.";
		LoggerService::error(msg);
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	std::shared_ptr<BaseResponse> res = nullptr;

	if (_itemManager->addItem(
		dto->getName(),
		static_cast<ItemType>(dto->getItemType() - 1)
	) == StockServer::StatusCode::OK
		) {
		std::string msg = "아이템이 추가되었습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), true, msg);
	}
	else {
		std::string msg = "아이템 추가에 실패했습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	if (res == nullptr) {
		std::string msg = "알 수 없는 에러가 발생했습니다.";
		LoggerService::error("add item 처리 중 알 수 없는 문제가 발생했습니다. : " + std::to_string(keyP.socketKey));
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::removeItem(RequestKeyPair keyP) {
	std::shared_ptr<RemoveItemDto> dto = DtoFactory::getRemoveItemDtoFromReq(keyP.req);

	if (dto == nullptr) {
		std::string msg = "입력 형식이 잘못되었습니다.";
		LoggerService::error(msg);
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	std::shared_ptr<BaseResponse> res = nullptr;
	std::string msg;
	std::shared_ptr<Stock> stock = _stockManager->findStockByItemId(dto->getItemId());

	if (stock != nullptr)
	{
		msg = "해당 아이템은 재고가 남아있어 삭제할 수 없습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);

		if (res == nullptr) {
			msg = "알 수 없는 에러가 발생했습니다.";
			return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
		}
		
		return res;
	}

	if (_itemManager->removeItem(
		dto->getItemId()
	) == StockServer::StatusCode::OK)
	{
		msg = std::to_string(dto->getItemId()) + " 아이템이 삭제되었습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), true, msg);
	}
	else
	{
		msg = "아이템을 삭제할 수 없습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	if (res == nullptr) {
		LoggerService::error("remove item 처리 중 알 수 없는 문제가 발생했습니다. : " + std::to_string(keyP.socketKey));
		msg = "알 수 없는 에러가 발생했습니다.";
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::printItemList(RequestKeyPair keyP) {
	std::string itemListStr = _itemManager->itemListToString();
	std::string msg = "success";

	std::shared_ptr<BaseResponse> res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), true, msg, itemListStr);
	
	if (res == nullptr) {
		LoggerService::error("print item list 처리 중 알 수 없는 문제가 발생했습니다. : " + std::to_string(keyP.socketKey));
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand());
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::addStock(RequestKeyPair keyP) {
	std::shared_ptr<AddStockDto> dto = DtoFactory::getAddStockDtoFromReq(keyP.req);

	if (dto == nullptr) {
		std::string msg = "입력 형식이 잘못되었습니다.";
		LoggerService::error(msg);
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	std::shared_ptr<BaseResponse> res = nullptr;
	std::string msg;

	std::shared_ptr<Item> item = _itemManager->findItemById(dto->getItemId());
	if (item == nullptr)
	{
		msg = "아이템이 존재하지 않습니다\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);

		if (res == nullptr) {
			LoggerService::error("add stock 처리 중 알 수 없는 문제가 발생했습니다. : " + std::to_string(keyP.socketKey));
			msg = "알 수 없는 에러가 발생했습니다.";
			return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
		}

		return res;
	}

	if (_stockManager->addStock(
		dto->getItemId(),
		dto->getCount()
	) == StockServer::StatusCode::OK)
	{
		msg = "재고가 늘어났습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), true, msg);
	}
	else
	{
		msg = "재고를 추가할 수 없습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	if (res == nullptr) {
		LoggerService::error("add stock 처리 중 알 수 없는 문제가 발생했습니다. : " + std::to_string(keyP.socketKey));
		msg = "알 수 없는 에러가 발생했습니다.";
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}
	
	return res;
}

std::shared_ptr<BaseResponse> DataManager::reduceStock(RequestKeyPair keyP) {
	std::shared_ptr<ReduceStockDto> dto = DtoFactory::getReduceStockDtoFromReq(keyP.req);

	if (dto == nullptr) {
		std::string msg = "입력 형식이 잘못되었습니다.";
		LoggerService::error(msg);
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	std::shared_ptr<BaseResponse> res = nullptr;
	std::string msg;

	if (_stockManager->reduceStock(
		dto->getItemId(),
		dto->getCount()
	) == StockServer::StatusCode::OK)
	{
		msg = "재고가 삭제되었습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), true, msg);
	}
	else
	{
		msg = "재고 삭제에 실패했습니다.\n";
		res = ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	if (res == nullptr) {
		LoggerService::error("reduce stock 처리 중 알 수 없는 문제가 발생했습니다. : " + std::to_string(keyP.socketKey));
		msg = "알 수 없는 에러가 발생했습니다.";
		return ResponseFactory::createResponseFromReq(keyP.req->getCommand(), false, msg);
	}

	return res;
}
