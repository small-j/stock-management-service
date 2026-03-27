#pragma once
#include "Status.h";
#include "NetworkManager.h"
#include "DataManager.h"

class NetworkManager;
class DataManager;

class App {
public:
	App() = default;
	virtual ~App() = default;

private:
	StockServer::ThreadStatus _appStatus;
	void setStatus(StockServer::ThreadStatus status) { _appStatus = status; };

protected:
	NetworkManager networkManager;
	DataManager dataManager;

public:
	void run();

	StockServer::ThreadStatus getStatus() { return _appStatus; };
};