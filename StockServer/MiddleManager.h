#pragma once
#include <memory>
#include <queue>
//#include "BaseRequest.h"

class DataManager;
class BaseRequest;

class MiddleManager {
public:
	MiddleManager() {}
	~MiddleManager();

private:
	bool _isQuitRequested = false;
	std::queue<std::shared_ptr<BaseRequest> > _jobQueue;
	// std::shared_ptr<DataManager> _dataManagerPtr;

public:
	bool isQuitRequested() {
		return _isQuitRequested;
	}
	void quit() {
		_isQuitRequested = true;
	}

	void initialize();
	bool loop(); // queue watching + pop job.
	bool execute(std::shared_ptr<BaseRequest> req);
	bool addRequest(std::shared_ptr<BaseRequest> req);

	// TODO: 
	//bool callApi();
	//std::shared_ptr<BaseResponse> makeResponse();
	//void pushJobToNextStepQueue();
};