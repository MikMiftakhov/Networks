#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

#define SERV_PORT 1605
#define BUFF_SIZE 128

using namespace std;

struct Person
{
	char name[25];
	double maht;
	int inf;
	int rus;
}st;

//работает максимально хуево, но стабильно 
int main()
{
	int erStat;
	Person st;

	WSADATA wsData;													//автоматически в момент создания загружаются данные о версии сокетов, используемых ОС, а также иная связанная системная информация
																	//Функция WSAStartup инициирует использование библиотеки DLL сокетов Windows процессом
	erStat = WSAStartup(0x0202, &wsData);							// в случае успеха возвращает 0, а в случае каких-то проблем возвращает код ошибки
	if (erStat != 0)
	{
		cout << "Error WinSock version initializaion #" << WSAGetLastError() << endl;
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;


	SOCKET ClientSock = socket(AF_INET, SOCK_DGRAM, 0);				//создание и инициализация сокета
	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket #" << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Client socket initialization is OK" << endl;

	sockaddr_in servInfo;

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);			//inet_pton помещает в ip_to_num нужный адрес
	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}

	ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(SERV_PORT);

	int len, n;
	char buff[BUFF_SIZE];
	while (1)
	{
		cout << "Press any key to continue or type stop for stop" << endl;
		string s;
		cin >> s;
		if (s == "stop") break;
		cout << "Enter a query: Last name, mathematics, computer science and Russian" << endl;
		cin >> st.name >> st.maht >> st.inf >> st.rus;
		sendto(ClientSock, (char*)&st, sizeof(st), 0, (sockaddr*)&servInfo, sizeof(servInfo));		//передача сообщения
		int len = (sizeof(servInfo));
		cout << "+++++++++++++" << endl;
		int n = recvfrom(ClientSock, buff, BUFF_SIZE, 0, (sockaddr*)&servInfo, &len);
		cout << "-------------" << endl;
		buff[n] = '\0';
		cout << buff << "\n";
		cout << endl;
	}
	cout << "exit" << endl;
	closesocket(ClientSock);
	WSACleanup();
	return 0;
}