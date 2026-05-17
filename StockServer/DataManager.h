#pragma once

#include "common.h"
#include "NetworkKeyPair.h"

class App;

class BaseRequest;
class BaseResponse;
class ItemManager;
class StockManager;

class DataManager {
public:
	DataManager(App* const app);
	virtual ~DataManager() = default;

private:
	App* const _owner;
	bool _isQuitRequested = false;
	std::mutex _jobQueueMutex;
	std::queue<RequestKeyPair> _jobQueue;

	std::shared_ptr<ItemManager> _itemManager;
	std::shared_ptr<StockManager> _stockManager;

public:
	bool isQuitRequested() {
		return _isQuitRequested;
	}
	void quit();
	void loop(); // queue watching + pop job.
	StockServer::StatusCode execute(RequestKeyPair keyP);
	StockServer::StatusCode addRequest(RequestKeyPair keyP);
	StockServer::StatusCode popRequest();
 
	//std::shared_ptr<BaseResponse> makeResponse();

	std::shared_ptr<BaseResponse> callApi(RequestKeyPair keyP);

	std::shared_ptr<BaseResponse> menus(RequestKeyPair keyP);
	std::shared_ptr<BaseResponse> printItemType(RequestKeyPair keyP);
	std::shared_ptr<BaseResponse> addItem(RequestKeyPair keyP);
	std::shared_ptr<BaseResponse> removeItem(RequestKeyPair keyP);
	std::shared_ptr<BaseResponse> printItemList(RequestKeyPair keyP);
	std::shared_ptr<BaseResponse> addStock(RequestKeyPair keyP);
	std::shared_ptr<BaseResponse> reduceStock(RequestKeyPair keyP);
};