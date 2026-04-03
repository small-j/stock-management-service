#pragma once
#include "Status.h"

class NetworkManager;
class DataManager;
class BaseRequest;
class BaseResponse;

class App {
public:
	App();
	virtual ~App() = default;

private:
	// 스레드의 상태 값을 저장하는 변수를 클래스 안에 두는게 맞는가?.
	StockServer::ThreadStatus _appStatus; 
	void setStatus(StockServer::ThreadStatus status) { _appStatus = status; };

protected:
	std::shared_ptr<NetworkManager> _networkManager;
	std::shared_ptr<DataManager> _dataManager;

public:
	void run();
	StockServer::StatusCode addRequest(int socketKey, std::shared_ptr<BaseRequest> req);
	StockServer::StatusCode addResponse(int socketKey, std::shared_ptr<BaseResponse> res);

	StockServer::ThreadStatus getStatus() { return _appStatus; };
};