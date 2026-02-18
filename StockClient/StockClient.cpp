
#include <iostream>
#include <winsock.h>
#include <charconv>
#include <limits>

#pragma comment(lib, "ws2_32.lib")

#define PORT 4578
#define PACKET_SIZE 1024 // byte

#define REQ_COMMAND_SIZE 2
#define RES_STATUS_SIZE 2 // 0: fail 1: success
#define RES_MESSAGE_SIZE 90 

#define SERVER_IP "127.0.0.1"

// method -> 1byte
// method -1 -> destroy connection
// method 1 -> print data
// method 2 -> input data
// method 3 -> print and input data
// 
// data -> 1023byte
// method에서 지정된 형태의 데이터가 들어가있다

void printMenu();
bool execute(SOCKET& serverSocket, short command);

void addItem(SOCKET& serverSocket);
void removeItem(SOCKET& serverSocket);
void printItemList(SOCKET& serverSocket);
void addStock(SOCKET& serverSocket);
void reduceStock(SOCKET& serverSocket);

bool isValidItemId(int itemId);
bool isValidStockCount(long long count);

int main()
{

	WSADATA  wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	SOCKET hSocket;
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	connect(hSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

	while (1)
	{
		printMenu();

		short command;
		std::cin >> command;	

		if (!execute(hSocket, command)) break;
	}

	closesocket(hSocket);

	WSACleanup();
	return 0;
}

void printMenu()
{
	std::cout << "1. 아이템 추가\n";
	std::cout << "2. 아이템 삭제\n";
	std::cout << "3. 아이템 목록 조회\n";
	std::cout << "4. 재고 추가\n";
	std::cout << "5. 재고 삭제\n";
	std::cout << "그 외. 종료\n";

	// 2차때 만들 기능
	// 4. 재고 csv 입력
	// 5. 아이템 삭제 -> 남은 재고는 우짜지?
	// 재고 입, 출고 기록 저장
	// 재고 주문 기록
}

bool execute(SOCKET& serverSocket, short command)
{
	switch (command)
	{
	case 1:
		addItem(serverSocket);
		return true;
	case 2:
		removeItem(serverSocket);
		return true;
	case 3:
		printItemList(serverSocket);
		return true;
	case 4:
		addStock(serverSocket);
		return true;
	/*case 5:
		reduceStock(serverSocket);
		return true;*/
	default:
		return false;
	}
}

void addItem(SOCKET& serverSocket)
{
	char sendBuffer[PACKET_SIZE] = {};
	char recvBuffer[PACKET_SIZE] = {};
	short resStatus = 0;
	short command = -1;
	std::string resMessage;

	memset(sendBuffer, '\0', PACKET_SIZE);
	memset(recvBuffer, '\0', PACKET_SIZE);

	// send + recv 아이템 타입 정보 서버로부터 수신.
	command = 6;
	memcpy(sendBuffer, &command, REQ_COMMAND_SIZE);
	send(serverSocket, sendBuffer, PACKET_SIZE, 0);
	recv(serverSocket, recvBuffer, PACKET_SIZE, 0);

	memcpy(&resStatus, recvBuffer, RES_STATUS_SIZE);
	resMessage.assign(recvBuffer + RES_STATUS_SIZE, RES_MESSAGE_SIZE);
	std::string itemTypeStr(recvBuffer + RES_STATUS_SIZE + RES_MESSAGE_SIZE);

	if (resStatus != 1) {
		std::cout << "아이템 타입을 가져오는데 실패했습니다. 다시 시도해주세요.";
	}

	// 필요한 데이터 입력 받기
	std::string name;
	int itemType = 0;
	std::cout << "아이템의 이름을 입력해주세요.\t";
	std::cin >> name;
	std::cout << "아이템의 타입의 번호를 선택해주세요.\n";
	std::cout << itemTypeStr;
	std::cin >> itemType;

	int nameLen = static_cast<int>(name.length());
	int offset = 0;

	// send + recv
	memset(sendBuffer, '\0', PACKET_SIZE);
	memset(recvBuffer, '\0', PACKET_SIZE);

	command = 1;
	
	memcpy(sendBuffer + offset, &command, sizeof(command));
	offset += REQ_COMMAND_SIZE;

	memcpy(sendBuffer + offset, &nameLen, sizeof(nameLen)); // name 길이 먼저 기록
	offset += sizeof(nameLen);

	memcpy(sendBuffer + offset, name.c_str(), nameLen); // name 기록
	offset += nameLen;

	memcpy(sendBuffer + offset, &itemType, sizeof(itemType)); // itemType 기록

	send(serverSocket, sendBuffer, PACKET_SIZE, 0);
	
	// 결과 수신
	recv(serverSocket, recvBuffer, PACKET_SIZE, 0);

	offset = 0;

	memcpy(&resStatus, recvBuffer + offset, RES_STATUS_SIZE);
	offset += RES_STATUS_SIZE;
	
	resMessage.assign(recvBuffer + offset, RES_MESSAGE_SIZE);
	offset += RES_MESSAGE_SIZE;
	
	std::string data(recvBuffer + offset);

	if (resStatus == 1)
		std::cout << data;
	else
		std::cout << resMessage;
}

void removeItem(SOCKET& serverSocket)
{
	char sendBuffer[PACKET_SIZE];
	char recvBuffer[PACKET_SIZE];
	short resStatus = 0;
	std::string resMessage;
	memset(sendBuffer, '\0', PACKET_SIZE);
	memset(recvBuffer, '\0', PACKET_SIZE);

	int itemId;
	std::cout << "아이템의 아이디를 입력해주세요.\t";
	std::cin >> itemId;

	if (isValidItemId(itemId) == false) {
		std::cout << "아이템 아이디가 올바르지 않습니다.\n";
		return;
	}

	int offset = 0;
	short command = 2;
	unsigned int castItemId = static_cast<unsigned int>(itemId);

	memcpy(sendBuffer + offset, &command, REQ_COMMAND_SIZE);
	offset += REQ_COMMAND_SIZE;

	memcpy(sendBuffer + offset, &castItemId, sizeof(castItemId));

	send(serverSocket, sendBuffer, PACKET_SIZE, 0);

	// 결과 수신
	recv(serverSocket, recvBuffer, PACKET_SIZE, 0);

	offset = 0;

	memcpy(&resStatus, recvBuffer + offset, RES_STATUS_SIZE);
	offset += RES_STATUS_SIZE;

	resMessage.assign(recvBuffer + offset, RES_MESSAGE_SIZE);
	offset += RES_MESSAGE_SIZE;

	std::string data(recvBuffer + offset);

	if (resStatus == 1)
		std::cout << data;
	else
		std::cout << resMessage;
}

void printItemList(SOCKET& serverSocket)
{
	char buffer[PACKET_SIZE] = {};
	short command = 3;
	memcpy(buffer, &command, REQ_COMMAND_SIZE);
	send(serverSocket, buffer, PACKET_SIZE, 0);

	char recvBuffer[PACKET_SIZE];
	recv(serverSocket, recvBuffer, PACKET_SIZE, 0);
	
	short resStatus;
	memcpy(&resStatus, recvBuffer, RES_STATUS_SIZE);
	std::string resMessage(recvBuffer + RES_STATUS_SIZE, RES_MESSAGE_SIZE);
	std::string data(recvBuffer + RES_STATUS_SIZE + RES_MESSAGE_SIZE);

	if (resStatus == 1)
		std::cout << data;
	else
		std::cout << resMessage;
}

void addStock(SOCKET& serverSocket)
{
	char sendBuffer[PACKET_SIZE];
	char recvBuffer[PACKET_SIZE];
	short resStatus = 0;
	std::string resMessage;
	memset(sendBuffer, '\0', PACKET_SIZE);
	memset(recvBuffer, '\0', PACKET_SIZE);

	int itemId;
	std::cout << "재고를 추가할 아이템 id를 입력해주세요.\t";
	std::cin >> itemId;

	long long count;
	std::cout << "재고 수를 입력해주세요.\t";
	std::cin >> count;

	if (isValidItemId(itemId) == false) {
		std::cout << "아이템 아이디가 올바르지 않습니다.\n";
		return;
	}
	if (isValidStockCount(count) == false) {
		std::cout << "아이템 아이디가 올바르지 않습니다.\n";
		return;
	}

	unsigned int castItemId = static_cast<unsigned int>(itemId);
	unsigned int castCount = static_cast<unsigned int>(count);

	int offset = 0;
	short command = 4;

	memcpy(sendBuffer + offset, &command, REQ_COMMAND_SIZE);
	offset += REQ_COMMAND_SIZE;

	memcpy(sendBuffer + offset, &castItemId, sizeof(castItemId));
	offset += sizeof(castItemId);

	memcpy(sendBuffer + offset, &castCount, sizeof(castCount));

	send(serverSocket, sendBuffer, PACKET_SIZE, 0);

	// 결과 수신
	recv(serverSocket, recvBuffer, PACKET_SIZE, 0);

	offset = 0;

	memcpy(&resStatus, recvBuffer + offset, RES_STATUS_SIZE);
	offset += RES_STATUS_SIZE;

	resMessage.assign(recvBuffer + offset, RES_MESSAGE_SIZE);
	offset += RES_MESSAGE_SIZE;

	std::string data(recvBuffer + offset);

	if (resStatus == 1)
		std::cout << data;
	else
		std::cout << resMessage;
}

//void reduceStock(SOCKET& serverSocket)
//{
//	long long itemId;
//	std::cout << "재고를 줄일 아이템 id를 입력해주세요.\t";
//	std::cin >> itemId;
//
//	if (itemId <= 0) {
//
//		return;
//	}
//
//	
//	std::cout << "삭제할 재고 수를 입력해주세요.\t";
//	std::cin >> 
//
//		// 재고 수 마이너스 체크
//	// send
//}

bool isValidItemId(int itemId)
{
	return 0 < itemId && itemId < 10000;
}

bool isValidStockCount(long long count)
{
	return 0 < count && count < (std::numeric_limits<unsigned int>::max)();
}
