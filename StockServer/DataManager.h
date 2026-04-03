#pragma once

#include <memory>
#include <queue>
#include <mutex>
#include <functional>

class App;

class BaseRequest;
class BaseResponse;
class ItemManager;
class StockManager;

class DataManager {
public:
	DataManager(App* app);
	virtual ~DataManager() = default;

private:
	App* _owner;
	bool _isQuitRequested = false;
	std::mutex _jobQueueMutex;
	std::queue<std::pair<int, std::shared_ptr<BaseRequest> > > _jobQueue;

	std::shared_ptr<ItemManager> _itemManager;
	std::shared_ptr<StockManager> _stockManager;

public:
	bool isQuitRequested() {
		return _isQuitRequested;
	}
	void quit();
	StockServer::StatusCode loop(); // queue wastching + pop job.
	StockServer::StatusCode execute(int socketKye, std::shared_ptr<BaseRequest> req);
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