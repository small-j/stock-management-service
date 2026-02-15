// StockServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"
#include <cstdint>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "ItemTypeHelper.h"
#include "ItemManager.h"
#include "StockManager.h"
#include "Stock.h" // Stock의 정의가 필요하므로 추가

using namespace std;

#define PORT 4578
#define PACKET_SIZE 1024

int8_t DESTROY_CONNECTION_METHOD = -1;
int8_t PRINT_METHOD = 1;
int8_t INPUT_METHOD = 2;
int8_t PRINT_AND_INPUT_METHOD = 3;

void ErrorHandler(const string& errMsg);
void run(SOCKET& clientSocket);
void printAndInputFromSocket(SOCKET clientSocket, string msg, char* recvBuffer);
void printFromSocket(SOCKET& clientSocket, string msg);
void printMenu(SOCKET& clientSocket);
bool execute(SOCKET& clientSocket, int8_t command, ItemManager& itemManager, StockManager& stockManager);
void addItem(SOCKET& clientSocket, ItemManager& itemManager);
void removeItem(SOCKET& clientSocket, ItemManager& itemManager, StockManager& stockManager);
void printItemList(SOCKET& clientSocket, ItemManager& itemManager);
void addStock(SOCKET& clientSocket, ItemManager& itemManager, StockManager& stockManager);
void reduceStock(SOCKET& clientSocket, StockManager& stockManager);


int main()
{
    WSADATA wsa;
    if (::WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        ErrorHandler("원속을 초기화 할 수 없습니다.");

    SOCKET listenSocket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
        ErrorHandler("접속 대기 소켓을 생성할 수 없습니다.");

    SOCKADDR_IN listenSocketAddr = { 0 };
    listenSocketAddr.sin_family = AF_INET;
    listenSocketAddr.sin_port = htons(PORT); // 요청을 수신할 소켓 프로세스가 사용할 포트번호
    listenSocketAddr.sin_addr.s_addr = htonl(INADDR_ANY); // 어떤 주소든 받는다.
    

    // 아래부터 예외처리 안해도 되나?

    // 소켓을 특정 주소 및 포트와 연결
    bind(listenSocket, (SOCKADDR*)&listenSocketAddr, sizeof(listenSocketAddr));
    // 연결 수신 대기 함수:listen
    listen(listenSocket, SOMAXCONN); // 소켓이 동시에 처리할 수 있는 최대 연결 수

    // 연결 클라이언트 정보 수신할 구조체
    SOCKADDR_IN acceptSocketAddr = {};
    int acceptSocketAddrSize = sizeof(acceptSocketAddr);
    // accept : 요청 연결 수락
    // 연결 수락되면서 연결된 클라이언트의 주소 정보도 기록됨.
    // 두번째 인수가 가리키는 버퍼의 크기를 지정?
    // accept는 들어오는 연결을 처리하는 소켓의 핸들을 반환한다. (새로 만들어진 소켓)
    // accept 함수가 실행되는 순간 프로그램의 실행이 그 지점에서 멈춤.
	// 대기열(Backlog)에 연결 요청이 들어올 때까지 CPU 자원을 거의 사용하지 않으면서 잠을 자는 상태가 됨.
	// 클라이언트가 connect를 호출하면(이 프로그램의 프로세스가 연결된 포트로) 
	// 운영체제가 이 연결 요청을 수락하고, accept 함수가 반환됨.

	SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&acceptSocketAddr, &acceptSocketAddrSize);
	cout << "클라이언트가 연결되었습니다.\n";

	run(clientSocket);

    closesocket(clientSocket);
    closesocket(listenSocket);

    WSACleanup();
    return 0;
}

void ErrorHandler(const string& errMsg)
{
	cout << errMsg << endl;
	::WSACleanup();
	exit(1);
}

void run(SOCKET& clientSocket) {
	char buffer[PACKET_SIZE] = {};

	ItemManager itemManager;
	StockManager stockManager;

	printMenu(clientSocket);

	//int command; // 만약 이 형태 그대로 배포가 되면 int를 절대 바꿀 수 없음.

	//while (recv(clientSocket, buffer, PACKET_SIZE, 0) > 0)
	while (INT32 res = recv(clientSocket, buffer, PACKET_SIZE, 0))
	{
		int8_t command = (int8_t)buffer[0] - '0';

		if (!execute(clientSocket, command, itemManager, stockManager))
		{
			char response[PACKET_SIZE];
			memset(response, '\0', PACKET_SIZE);
			memcpy(response, &DESTROY_CONNECTION_METHOD, sizeof(DESTROY_CONNECTION_METHOD));

			send(clientSocket, response, sizeof(response), 0);
			break;
		}

		printMenu(clientSocket);
	}
}

void printAndInputFromSocket(SOCKET clientSocket, string msg, char* recvBuffer)
{
	char sendBuffer[PACKET_SIZE];

	memset(sendBuffer, '\0', PACKET_SIZE);
	memcpy(sendBuffer, &PRINT_AND_INPUT_METHOD, sizeof(int8_t));
	memcpy(sendBuffer + sizeof(int8_t), msg.c_str(), msg.length());
	send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
	recv(clientSocket, recvBuffer, PACKET_SIZE, 0);
}

void printFromSocket(SOCKET& clientSocket, string msg)
{
	char sendBuffer[PACKET_SIZE];
	memset(sendBuffer, '\0', PACKET_SIZE);
	memcpy(sendBuffer, &PRINT_METHOD, sizeof(int8_t));
	memcpy(sendBuffer + sizeof(int8_t), msg.c_str(), msg.length());
	send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
}

void printMenu(SOCKET& clientSocket)
{
	string msg;
	
	msg += "1. 아이템 추가\n";
	msg += "2. 아이템 삭제\n";
	msg += "3. 아이템 목록 조회\n";
	msg += "4. 재고 추가\n";
	msg += "5. 재고 삭제\n";
	msg += "그 외. 종료\n";


	char sendBuffer[PACKET_SIZE]; // 첫번쨰부터 마지막까지 이 값으로 초기화 됨.
	// 초기화해야하는 이유 : 
	//memset로 패킷나머지 사이즈만큼 채워준다? null
	memset(sendBuffer, '\0', PACKET_SIZE);
	memcpy(sendBuffer, &PRINT_AND_INPUT_METHOD, sizeof(int8_t));
	memcpy(sendBuffer + sizeof(int8_t), msg.c_str(), msg.length());

	send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);

	// 2차때 만들 기능
	// 4. 재고 csv 입력
	// 5. 아이템 삭제 -> 남은 재고는 우짜지?
	// 재고 입, 출고 기록 저장
	// 재고 주문 기록
}

bool execute(SOCKET& clientSocket, int8_t command, ItemManager& itemManager, StockManager& stockManager)
{
	switch (command)
	{
	case 1:
		addItem(clientSocket, itemManager);
		return true;
	case 2:
		removeItem(clientSocket, itemManager, stockManager);
		return true;
	case 3:
		printItemList(clientSocket, itemManager);
		return true;
	case 4:
		addStock(clientSocket, itemManager, stockManager);
		return true;
	case 5:
		reduceStock(clientSocket, stockManager);
		return true;
	default:
		return false;
	}
}

void addItem(SOCKET& clientSocket, ItemManager& itemManager)
{
	char recvBuffer[PACKET_SIZE];
	string msg;
	string name;
	int itemType;

	msg = "아이템의 이름을 입력해주세요.\t";
	printAndInputFromSocket(clientSocket, msg, recvBuffer);
	name = string(recvBuffer);

	msg = "아이템의 타입의 번호를 선택해주세요.\n";
	msg += ItemTypeHelper::getAllItemInfoToString();
	printAndInputFromSocket(clientSocket, msg, recvBuffer);
	itemType = (int)recvBuffer[0] - '0'; // TODO: 이렇게 하면 안되는데.. 어카지..

	itemType -= 1;
	if (itemManager.addItem(name, static_cast<ItemType>(itemType)))
	{
		msg = "아이템이 추가되었습니다.\n";
		printFromSocket(clientSocket, msg);
		return;
	}

	msg = "아이템 추가에 실패했습니다.\n";
	printFromSocket(clientSocket, msg);
}

void removeItem(SOCKET& clientSocket, ItemManager& itemManager, StockManager& stockManager)
{
	unsigned int itemId;
	cin >> itemId;

	shared_ptr<Stock> stock = stockManager.findStockByItemId(itemId);
	if (stock != nullptr)
	{
		cout << "해당 아이템은 재고가 남아있어 삭제할 수 없습니다.\n";
		return;
	}

	if (itemManager.removeItem(itemId))
	{
		cout << itemId << "아이템이 삭제되었습니다.\n";
	}
	else
	{
		cout << "아이템을 삭제할 수 없습니다.\n";
	}
}

void printItemList(SOCKET& clientSocket, ItemManager& itemManager)
{
	std::string itemListStr = itemManager.itemListToString();
	printFromSocket(clientSocket, itemListStr);
}

void addStock(SOCKET& clientSocket, ItemManager& itemManager, StockManager& stockManager)
{
	unsigned int itemId = 0; // 선
	cout << "재고를 추가할 아이템 id를 입력해주세요.\t";
	cin >> itemId;

	shared_ptr<Item> item = itemManager.findItemById(itemId);
	if (item == nullptr)
	{
		cout << "아이템이 존재하지 않습니다\n";
		return;
	}

	unsigned int count;
	cout << "재고 수를 입력해주세요.\t";
	cin >> count;
	if (stockManager.addStock(itemId, count))
	{
		cout << "재고가 늘어났습니다.\n";

		shared_ptr<Stock> stock = stockManager.findStockByItemId(itemId);
		if (stock == nullptr)
		{
			cout << "재고 조회에 실패했습니다. 다시 시도해주세요.\n";
			return;
		}
		cout << stock->getItemId() << ": " << stock->getCount() << endl;
	}
	else
	{
		cout << "재고를 추가할 수 없습니다.\n";
	}
}

void reduceStock(SOCKET& clientSocket, StockManager& stockManager)
{
	unsigned int itemId;
	unsigned int count;
	cout << "재고를 삭제할 아이템 id를 입력해주세요.\t";
	cin >> itemId;

	// item id 체크
	cout << "삭제할 재고 수를 입력해주세요.\t";
	cin >> count;

	if (stockManager.reduceStock(itemId, count))
	{
		cout << "재고가 삭제되었습니다.\n";
		if (shared_ptr<Stock> stock = stockManager.findStockByItemId(itemId)) {
			cout << stock->getItemId() << "재고 수 : " << stock->getCount();
		}
		else {
			cout << "재고 조회에 실패했습니다. 다시 시도해주세요.\n";
		}
	}
	else
	{
		cout << "재고 삭제에 실패했습니다.\n";
	}
}
