#pragma once
#include <winsock.h>
#include <memory>

class BaseRequest;
class BaseResponse;

class DataManager {
public:
	void sendToServer(SOCKET& serverSocket, BaseRequest& req, BaseResponse& res);
};
