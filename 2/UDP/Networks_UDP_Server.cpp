#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")

#define SERV_PORT 1605
#define BUFF_SIZE 128

using namespace std;

const string ex = "excellent";
const string gd = "good";
const string nm = "normal";
const string bd = "bad";
const string sb = "sad";

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable: 4996)

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
	int erStat;														// для ошибок												
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



	SOCKET ServSock = socket(AF_INET, SOCK_DGRAM, 0);				//создание и инициализация сокета
	if (ServSock == INVALID_SOCKET) {
		cout << "Error initialization socket #" << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;



	sockaddr_in servInfo;														//структура  с доп информацией для сокета, например ip-адресом и портом
	ZeroMemory(&servInfo, sizeof(servInfo));

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);						//inet_pton помещает в ip_to_num нужный адрес
	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}

	servInfo.sin_family = AF_INET;
	servInfo.sin_port = htons(SERV_PORT);
	servInfo.sin_addr.s_addr = INADDR_ANY;

	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));						//привязываем доп инфу к сокету 
	if (erStat != 0) {
		cout << "Error Socket binding to server info. Error #" << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Binding socket to Server info is OK" << endl;


	char buff[BUFF_SIZE];
	while (1)
	{
		sockaddr_in clientInfo;
		ZeroMemory(&clientInfo, sizeof(clientInfo));
		int clientInfo_size = sizeof(clientInfo);
		int bsize2 = recvfrom(ServSock, (char*)&st, sizeof(st), 0, (sockaddr*)&clientInfo, &clientInfo_size);
		cout << st.name << " " << st.maht << " " << st.inf << " " << st.rus << endl;
		hostent* hst;
		hst = gethostbyaddr((char*)&clientInfo.sin_addr, 4, AF_INET);
		cout << "NEW DATAGRAM!\n" << ((hst) ? hst->h_name : "Unkown host") << "\n" << inet_ntoa(clientInfo.sin_addr) << "\n" << ntohs(clientInfo.sin_port) << '\n';

		int score = (st.maht + st.inf + st.rus) / 3;
		if (score >= 90)
			sendto(ServSock, (char*)&ex[0], ex.size(), 0, (sockaddr*)&clientInfo, clientInfo_size);
		if (score >= 80 && score < 90)
			sendto(ServSock, (char*)&gd[0], gd.size(), 0, (sockaddr*)&clientInfo, clientInfo_size);
		if (score >= 60 && score < 80)
			sendto(ServSock, (char*)&nm[0], nm.size(), 0, (sockaddr*)&clientInfo, clientInfo_size);
		if (score >= 40 && score < 60)
			sendto(ServSock, (char*)&bd[0], bd.size(), 0, (sockaddr*)&clientInfo, clientInfo_size);
		if (score >= 0 && score < 40)
			sendto(ServSock, (char*)&sb[0], sb.size(), 0, (sockaddr*)&clientInfo, clientInfo_size);
		cout << endl;
		cout << "ready" << endl;
	}														//закрытие сокета
}