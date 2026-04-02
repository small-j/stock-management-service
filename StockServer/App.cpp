#include "pch.h"
#include "App.h"
#include <thread>

App::App() :_appStatus(StockServer::ThreadStatus::INIT),
	networkManager(this),
	dataManager(this) {
}

void App::run() {
	std::thread dataManagerT(&DataManager::loop, &dataManager);

	std::thread networkManagerListenT(&NetworkManager::listenRequest, &networkManager);
	std::thread networkManagerResponseT(&NetworkManager::loop, &networkManager);

	setStatus(StockServer::ThreadStatus::WORKING);

	while (true) { 
		// if () break; // TODO: 유저 입력(ctrl + C) 받을때까지 while문 유지.
	}

	networkManager.quit();
	networkManagerListenT.join();
	networkManagerResponseT.join();

	dataManager.quit();
	dataManagerT.join(); // dataManagerT 스레드가 끝나기를 기다림.

	setStatus(StockServer::ThreadStatus::QUIT);
}

StockServer::StatusCode App::addRequest(int socketKey, std::shared_ptr<BaseRequest> req) {
	return dataManager.addRequest(socketKey, req);
}

StockServer::StatusCode App::addResponse(int socketKey, std::shared_ptr<BaseResponse> res) {
	return networkManager.addResponse(socketKey, res);
}
