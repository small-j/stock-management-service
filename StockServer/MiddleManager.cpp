#include "pch.h"
#include "MiddleManager.h"
#include "BaseRequest.h"
#include <thread>
#include <chrono>

void MiddleManager::quit() {
	_isQuitRequested = true;
}

StockServer::StatusCode MiddleManager::loop() {
	while (isQuitRequested() == false) {
		if (_jobQueue.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); // cpu 점유 방지.
		}
		else {
			execute(_jobQueue.front());

			// TODO: popRequest 실패했을 때 어떻게 할지?
			popRequest();
		}
	}

	return StockServer::StatusCode::OK;
}

StockServer::StatusCode MiddleManager::execute(std::shared_ptr<BaseRequest> req) {
	if (!req) return StockServer::StatusCode::CANCELLED;
	std::cout << req->getCommand() << "추출 성공!" << std::endl; // TODO : 변경
	return StockServer::StatusCode::OK;
}

StockServer::StatusCode MiddleManager::addRequest(std::shared_ptr<BaseRequest> req) {
	if (!req) return StockServer::StatusCode::CANCELLED;

	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	_jobQueue.push(req);
	return StockServer::StatusCode::OK;
}

StockServer::StatusCode MiddleManager::popRequest() {
	std::lock_guard<std::mutex> lock(_jobQueueMutex);

	_jobQueue.pop();

	return StockServer::StatusCode::OK;
}
