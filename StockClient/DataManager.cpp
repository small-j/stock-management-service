#include <iostream>
#include "DataManager.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include <charconv>

#define PACKET_SIZE 1024 // byte
// 1024로 고정되어 있어서 이 이상의 데이터를 보낼 수가 없다.
// 어떻게 고쳐야할지 모르겠는데 
// 지금도 고쳐야할게 많으니까 나중에 추가 공부를 해보자

// 나중에는 send, receive 나누고 queue에 각각 넣어줄 것이다.
void DataManager::sendToServer(SOCKET& serverSocket, BaseRequest& req, BaseResponse& res) {
	char sendBuffer[PACKET_SIZE] = {};

	req.serialize(sendBuffer);
	send(serverSocket, sendBuffer, PACKET_SIZE, 0);

	char recvBuffer[PACKET_SIZE];
	recv(serverSocket, recvBuffer, PACKET_SIZE, 0);

	res.deserialize(recvBuffer);
}