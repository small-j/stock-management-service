#pragma once
#include <memory>
#include <queue>
#include <mutex>
//#include "BaseRequest.h"

class BaseRequest;

class MiddleManager {
public:
	MiddleManager() = default;
	virtual ~MiddleManager() = default;

private:
	bool _isQuitRequested = false;
	std::mutex _jobQueueMutex;
	std::queue<std::shared_ptr<BaseRequest> > _jobQueue;

public:
	bool isQuitRequested() {
		return _isQuitRequested;
	}
	void quit();
	StockServer::StatusCode loop(); // queue watching + pop job.
	StockServer::StatusCode execute(std::shared_ptr<BaseRequest> req);
	StockServer::StatusCode addRequest(std::shared_ptr<BaseRequest> req);
	StockServer::StatusCode popRequest();

	// TODO: 
	//StockServer::StatusCode callApi();
	//std::shared_ptr<BaseResponse> makeResponse();
};