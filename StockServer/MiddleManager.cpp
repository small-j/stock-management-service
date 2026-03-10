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

bool MiddleManager::loop() {
	while (isQuitRequested() == false) {
		if (_jobQueue.empty()) {
			std::this_thread::sleep_for(std::chrono::milliseconds(500)); // cpu 점유 방지.
		}
		else {
			execute(_jobQueue.front());

			popRequest();
		}
	}

	return true;
}

// TODO: 어떤 유형의 문제인지를 구분해주자 
// -> enum 하나 선언해서 에러 타입을 구분
bool MiddleManager::execute(std::shared_ptr<BaseRequest> req) {
	if (!req) return false;
	std::cout << req->getCommand() << "추출 성공!" << std::endl;
	return true;
}

bool MiddleManager::addRequest(std::shared_ptr<BaseRequest> req) {
	if (!req) return false;

	std::lock_guard<std::mutex> lock(_jobQueueMutex);
	_jobQueue.push(req);

	// 넣는 속도보다 빼는 속도가 현저하게 느리면 계속 쌓일수밖에 없고
	// 큐에 넣는 걸 실패할 수 있다. 
	// -> TODO: push작업이 실패하는 경우 어떤 결과가 반환되는지 알아봐야겠다.

	return true;
}

bool MiddleManager::popRequest() {
	std::lock_guard<std::mutex> lock(_jobQueueMutex);

	_jobQueue.pop();

	return true;
}
