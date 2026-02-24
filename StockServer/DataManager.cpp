#include "pch.h"
#include <iostream>
#include "DataManager.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include "RequestCommand.h"
#include <charconv>

#define PACKET_SIZE 1024 // byte
// 1024로 고정되어 있어서 이 이상의 데이터를 보낼 수가 없다.
// 어떻게 고쳐야할지 모르겠는데 
// 지금도 고쳐야할게 많으니까 나중에 추가 공부를 해보자

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

