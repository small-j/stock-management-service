#include <iostream>
#include <cassert>
#include "DataManager.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include <charconv>
#include "RequestCommand.h"
#include "AddItemResponse.h"
#include "RemoveItemResponse.h"
#include "PrintItemResponse.h"
#include "GetItemTypesResponse.h"
#include "AddStockResponse.h"
#include "ReduceStockResponse.h"
#include "GetMenusResponse.h"

// 나중에는 send, receive 나누고 queue에 각각 넣어줄 것이다.
void DataManager::sendToServer(SOCKET& serverSocket, BaseRequest& req) {
	char sendBuffer[PACKET_SIZE];
	memset(sendBuffer, '\0', PACKET_SIZE);

	req.serialize(sendBuffer);
	send(serverSocket, sendBuffer, PACKET_SIZE, 0);
}

std::shared_ptr<BaseResponse> DataManager::recieveFromServer(SOCKET& socket) {
	char recvBuffer[PACKET_SIZE];
	memset(recvBuffer, '\0', PACKET_SIZE);
	INT32 result = recv(socket, recvBuffer, PACKET_SIZE, 0);

	if (result == -1 || result == 0) return nullptr;

	BaseResponse baseRes(Request::Command::UNKNOWN, true);
	baseRes.deserialize(recvBuffer);

	std::shared_ptr<BaseResponse> res = createResponseFromCommand(baseRes.getCommand());
	if (!res) {
		// log
		return nullptr;
	}
	
	if (res->deserialize(recvBuffer) == 0) {
		return nullptr;
	}

	return res;
}

std::shared_ptr<BaseResponse> DataManager::createResponseFromCommand(short cmd) {
	switch (cmd) {
	case Request::Command::ADD_ITEM:
		return std::make_shared<AddItemResponse>();
	case Request::Command::REMOVE_ITEM:
		return std::make_shared<RemoveItemResponse>();
	case Request::Command::GET_ITEM_TYPE:
		return std::make_shared<GetItemTypesResponse>();
	case Request::Command::PRINT_ITEM:
		return std::make_shared<PrintItemResponse>();
	case Request::Command::ADD_STOCK:
		return std::make_shared<AddStockResponse>();
	case Request::Command::REDUCE_STOCK:
		return std::make_shared<ReduceStockResponse>();
	case Request::Command::GET_MENU:
		return std::make_shared<GetMenusResponse>();
	default:
		assert(false);
		return nullptr;
	}
}
