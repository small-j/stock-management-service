
#include <iostream>
#include <winsock.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 4578
#define PACKET_SIZE 1024 // byte
#define SERVER_IP "127.0.0.1"

// method -> 1byte
// method -1 -> destroy connection
// method 1 -> print data
// method 2 -> input data
// method 3 -> print and input data
// 
// data -> 1023byte
// method에서 지정된 형태의 데이터가 들어가있다

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

	int8_t DESTROY_CONNECTION_METHOD = -1;
	int8_t PRINT_METHOD = 1;
	int8_t INPUT_METHOD = 2;
	int8_t PRINT_AND_INPUT_METHOD = 3;

	while (1)
	{
		char buffer[PACKET_SIZE] = {};
		recv(hSocket, buffer, PACKET_SIZE, 0);

		// 구분자를 주면 구분자를 기준으로 string을 자르고 앞은 command, 뒤는 data 이렇게 쓰기도 한다.
		// 단 이 경우 구분자를 절대 데이터나 command 값으로 사용할 수 없다.
		int8_t method = (int8_t)buffer[0];

		if (method == DESTROY_CONNECTION_METHOD) break;

		if (method == PRINT_METHOD || method == PRINT_AND_INPUT_METHOD) {
			// command를 제외하고 나머지를 출력해야함.
			std::string dataPart(buffer + sizeof(method));
			std::cout << dataPart << std::endl;
		}

		if (method == PRINT_AND_INPUT_METHOD) {
			std::string input;
			std::cin >> input;

			// method + data
			char sendBuffer[PACKET_SIZE] = {};
			memset(sendBuffer, '\0', PACKET_SIZE);
			memcpy(sendBuffer, input.c_str(), input.length());

			send(hSocket, sendBuffer, sizeof(sendBuffer), 0);
		}

	}


	closesocket(hSocket);

	WSACleanup();
	return 0;
}
