#pragma once
#include <memory>
#include <queue>
#include <mutex>
#include <functional>

#include "ItemManager.h"
#include "StockManager.h"

// TODO: 삭제
#include "NetworkManager.h"

class BaseRequest;
class BaseResponse;
class ItemManager;
class StockManager;

class NetworkManager;

class DataManager {
public:
	DataManager() = default;
	virtual ~DataManager() = default;

private:
	bool _isQuitRequested = false;
	std::mutex _jobQueueMutex;
	std::queue<std::pair<int, std::shared_ptr<BaseRequest> > > _jobQueue;

	ItemManager itemManager;
	StockManager stockManager;

public:
	bool isQuitRequested() {
		return _isQuitRequested;
	}
	void quit();
	StockServer::StatusCode loop(NetworkManager& networkManager); // queue wastching + pop job.
	StockServer::StatusCode execute(int socketKye, std::shared_ptr<BaseRequest> req, NetworkManager& networkManager);
	StockServer::StatusCode addRequest(int socketKey, std::shared_ptr<BaseRequest> req);
	StockServer::StatusCode popRequest();
 
	//std::shared_ptr<BaseResponse> makeResponse();

	std::shared_ptr<BaseResponse> callApi(int socketKey, std::shared_ptr<BaseRequest> req);

	std::shared_ptr<BaseResponse> menus(int socketKey);
	std::shared_ptr<BaseResponse> printItemType(int socketKey);
	std::shared_ptr<BaseResponse> addItem(int socketKey,std::shared_ptr<BaseRequest> req);
	std::shared_ptr<BaseResponse> removeItem(int socketKey, std::shared_ptr<BaseRequest> req);
	std::shared_ptr<BaseResponse> printItemList(int socketKey);
	std::shared_ptr<BaseResponse> addStock(int socketKey, std::shared_ptr<BaseRequest> req);
	std::shared_ptr<BaseResponse> reduceStock(int socketKey, std::shared_ptr<BaseRequest> req);
};