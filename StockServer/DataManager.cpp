#include "pch.h"
#include <iostream>
#include <cassert>
#include "DataManager.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include "RequestCommand.h"
#include <charconv>
#include "AddItemRequest.h"
#include "RemoveItemRequest.h"
#include "PrintItemRequest.h"
#include "GetItemTypesRequest.h"
#include "AddStockRequest.h"
#include "ReduceStockRequest.h"
#include "GetMenusRequest.h"

#include<queue>

// 나중에는 send, receive 나누고 queue에 각각 넣어줄 것이다.
void DataManager::sendToClient(SOCKET& socket, BaseResponse& res) {
	//int maxDataSize = PACKET_SIZE - RES_STATUS_SIZE - RES_MESSAGE_SIZE;

	//if (data != std::nullopt 
	//	&& data.value().size() > maxDataSize) {
	//	throw std::out_of_range("데이터 크기가 버퍼 용량을 초과했습니다."); // 질문
	//}
	char sendBuffer[PACKET_SIZE];
	memset(sendBuffer, '\0', PACKET_SIZE);

	res.serialize(sendBuffer);
	send(socket, sendBuffer, PACKET_SIZE, 0);
}

std::shared_ptr<BaseRequest> DataManager::recieveFromClient(SOCKET& socket) {
	// char recvBuffer[PACKET_SIZE];
	char recvBuffer[PACKET_SIZE];
	memset(recvBuffer, '\0', PACKET_SIZE);

	INT32 result = recv(socket, recvBuffer, PACKET_SIZE, 0);

	if (result == -1 || result == 0) return nullptr;

	BaseRequest baseReq(Request::Command::UNKNOWN);
	baseReq.deserialize(recvBuffer);

	std::shared_ptr<BaseRequest> req = createRequestFromCommand(baseReq.getCommand());
	if (!req) {
		// TODO: log
		return nullptr;
	}

	if (req->deserialize(recvBuffer) == 0) {
		return nullptr;
	}

	return req;
}

std::shared_ptr<BaseRequest> DataManager::createRequestFromCommand(short cmd) {
	switch (cmd) {
	case Request::Command::ADD_ITEM:
		return std::make_shared<AddItemRequest>();
	case Request::Command::REMOVE_ITEM :
		return std::make_shared<RemoveItemRequest>();
	case Request::Command::GET_ITEM_TYPE:
		return std::make_shared<GetItemTypesRequest>();
	case Request::Command::PRINT_ITEM:
		return std::make_shared<PrintItemRequest>();
	case Request::Command::ADD_STOCK:
		return std::make_shared<AddStockRequest>();
	case Request::Command::REDUCE_STOCK:
		return std::make_shared<ReduceStockRequest>();
	case Request::Command::GET_MENU:
		return std::make_shared<GetMenusRequest>();
	default:
		assert(false);
		return nullptr;
	}
}
