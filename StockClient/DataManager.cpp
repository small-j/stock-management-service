#include <iostream>
#include "DataManager.h"
#include "BaseRequest.h"
#include "BaseResponse.h"
#include <charconv>

// 나중에는 send, receive 나누고 queue에 각각 넣어줄 것이다.
void DataManager::sendToServer(SOCKET& serverSocket, BaseRequest& req, BaseResponse& res) {
	char sendBuffer[PACKET_SIZE];
	memset(sendBuffer, '\0', PACKET_SIZE);

	req.serialize(sendBuffer);
	send(serverSocket, sendBuffer, PACKET_SIZE, 0);

	char recvBuffer[PACKET_SIZE];
	memset(recvBuffer, '\0', PACKET_SIZE);
	recv(serverSocket, recvBuffer, PACKET_SIZE, 0);

	res.deserialize(recvBuffer);
}