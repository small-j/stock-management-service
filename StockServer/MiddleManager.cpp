#include "pch.h"
#include "MiddleManager.h"
#include "BaseRequest.h"
#include <thread>
#include <chrono>

MiddleManager::~MiddleManager() {
	// 사용 메모리 및 변수 정리를 위한 로직이기 때문에 소멸자에서 수행.
	quit();
}

void MiddleManager::initialize() {
	loop();
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
	std::cout << req->getCommand() << "추출 성공!" << std::endl;
	return StockServer::StatusCode::OK;
}

StockServer::StatusCode MiddleManager::addRequest(std::shared_ptr<BaseRequest> req) {
	if (!req) return StockServer::StatusCode::CANCELLED;

	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	_jobQueue.push(req);

	// 넣는 속도보다 빼는 속도가 현저하게 느리면 계속 쌓일수밖에 없고
	// 큐에 넣는 걸 실패할 수 있다. 
	// -> TODO: push작업이 실패하는 경우 어떤 결과가 반환되는지 알아봐야겠다.

	return StockServer::StatusCode::OK;
}

StockServer::StatusCode MiddleManager::popRequest() {
	std::lock_guard<std::mutex> lock(_jobQueueMutex);

	_jobQueue.pop();

	return StockServer::StatusCode::OK;
}
