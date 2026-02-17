
#include <iostream>
#include <winsock.h>

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
	/*case 1:
		addItem(serverSocket);
		return true;
	case 2:
		removeItem(serverSocket);
		return true;*/
	case 3:
		printItemList(serverSocket);
		return true;
	/*case 4:
		addStock(serverSocket);
		return true;
	case 5:
		reduceStock(serverSocket);
		return true;*/
	default:
		return false;
	}
}

//void addItem(SOCKET& serverSocket)
//{
//	char recvBuffer[PACKET_SIZE];
//	string msg;
//	string name;
//	int itemType = 0;
//
//	msg = "아이템의 이름을 입력해주세요.\t";
//	printAndInputFromSocket(clientSocket, msg, recvBuffer);
//	name = string(recvBuffer);
//
//	msg = "아이템의 타입의 번호를 선택해주세요.\n";
//	msg += ItemTypeHelper::getAllItemInfoToString();
//	printAndInputFromSocket(clientSocket, msg, recvBuffer);
//
//
//}
//
//void removeItem(SOCKET& serverSocket)
//{
//	string msg;
//	char recvBuffer[PACKET_SIZE];
//
//	msg = "아이템의 아이디를 입력해주세요.\t";
//	printAndInputFromSocket(clientSocket, msg, recvBuffer);
//
//	unsigned int itemId;
//	auto [ptr, ec] = std::from_chars(recvBuffer, recvBuffer + std::strlen(recvBuffer), itemId);
//
//	if (ec != std::errc())
//	{
//		msg = "아이디 입력이 잘못되었습니다.\n";
//		printFromSocket(clientSocket, msg);
//		return;
//	}
//
//}


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

//void addStock(SOCKET& serverSocket)
//{
//	string msg;
//	char recvBuffer[PACKET_SIZE];
//	std::from_chars_result from_char;
//
//	msg = "재고를 추가할 아이템 id를 입력해주세요.\t";
//	printAndInputFromSocket(clientSocket, msg, recvBuffer);
//
//	unsigned int itemId = 0;
//	from_char = std::from_chars(recvBuffer, recvBuffer + std::strlen(recvBuffer), itemId);
//
//	if (from_char.ec != std::errc())
//	{
//		msg = "아이디 입력이 잘못되었습니다.\n";
//		printFromSocket(clientSocket, msg);
//		return;
//	}
//
//
//}
//
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
