#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <iostream>
#include <windows.h>
//CLIENT
#pragma warning(disable: 4996)

SOCKET Connection;
bool threadAlive = 0;


void ClientHandler() {
	char msg[256];

	while (true)
	{
		if (recv(Connection, msg, sizeof(msg), NULL) > 0) {
			std::cout << msg << std::endl;
		}
		else {
			threadAlive = 0;
			return;
		}
	}
}

void typingCommand()
{
	char msg1[256];
	while (true) {
		std::cin.getline(msg1, sizeof(msg1));
		send(Connection, msg1, sizeof(msg1), NULL);
		Sleep(10);
	}
}

int main(int argc, char* argv[]) {
	//WSAStartup
	std::cout << "Start\n";
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0) {
		std::cout << "Error" << std::endl;
		exit(1);
	}

	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	Connection = socket(AF_INET, SOCK_STREAM, NULL);


	while (TRUE)
	{
		if (connect(Connection, (SOCKADDR*)&addr, sizeof(addr)) == 0 && threadAlive == 0)
		{
			threadAlive = 1;
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)typingCommand, NULL, NULL, NULL);
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ClientHandler, NULL, NULL, NULL);
		}
		else {
			connect(Connection, (SOCKADDR*)&addr, sizeof(addr));
		}
		Sleep(100);
	}
	system("pause");
	return 0;
}