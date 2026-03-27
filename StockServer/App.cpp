#include "App.h"
#include <thread>

void App::run() {
	std::thread dataManagerT(&DataManager::loop, &dataManager, NetworkManager::addResponse);

	std::thread networkManagerListenT(&NetworkManager::listenRequest, &networkManager, DataManager::addRequest);
	std::thread networkManagerResponseT(&NetworkManager::loop, &networkManager);

	setStatus(StockServer::ThreadStatus::WORKING);
	while (true) { 
		if () break; // 유저 입력(ctrl + C) 받을때까지 while문 유지.
	}

	networkManager.quit();
	networkManagerListenT.join();
	networkManagerResponseT.join();

	dataManager.quit();
	dataManagerT.join(); // dataManagerT 스레드가 끝나기를 기다림.

	setStatus(StockServer::ThreadStatus::QUIT);
}
