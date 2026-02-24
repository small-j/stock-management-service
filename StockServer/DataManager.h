#pragma once
#include <winsock.h>
//#include "RequestCommand.h"

class BaseRequest;
class BaseResponse;

class DataManager {
public:
	void sendToClient(SOCKET& socket, BaseResponse& res);
	bool recieveFromClient(SOCKET& socket, char* buffer);
};
