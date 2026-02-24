#include "pch.h"
#include <iostream>
#include "DataManager.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include "RequestCommand.h"
#include <charconv>

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

bool DataManager::recieveFromClient(SOCKET& socket, char* recvBuffer) {
	// char recvBuffer[PACKET_SIZE];
	memset(recvBuffer, '\0', PACKET_SIZE);
	INT32 res = recv(socket, recvBuffer, PACKET_SIZE, 0);

	return res != -1;
}

