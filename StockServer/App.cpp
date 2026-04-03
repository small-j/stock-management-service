#include "pch.h"
#include <thread>

#include "App.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include "NetworkManager.h"
#include "DataManager.h"

App::App() :_appStatus(StockServer::ThreadStatus::INIT) {
	_networkManager = std::make_shared<NetworkManager>(this);
	_dataManager = std::make_shared<DataManager>(this);
}

void App::run() {
	std::thread dataManagerT(&DataManager::loop, _dataManager);

	std::thread networkManagerListenT(&NetworkManager::listenRequest, _networkManager);
	std::thread networkManagerResponseT(&NetworkManager::loop, _networkManager);

	setStatus(StockServer::ThreadStatus::WORKING);

	while (true) { 
		// if () break; // TODO: 유저 입력(ctrl + C) 받을때까지 while문 유지.
	}

	_networkManager->quit();
	networkManagerListenT.join();
	networkManagerResponseT.join();

	_dataManager->quit();
	dataManagerT.join();

	setStatus(StockServer::ThreadStatus::QUIT);
}

StockServer::StatusCode App::addRequest(int socketKey, std::shared_ptr<BaseRequest> req) {
	return _dataManager->addRequest(socketKey, req);
}

StockServer::StatusCode App::addResponse(int socketKey, std::shared_ptr<BaseResponse> res) {
	return _networkManager->addResponse(socketKey, res);
}
