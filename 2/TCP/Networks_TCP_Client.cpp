#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

#define SERV_PORT 1605
#define client_port 1505
#define BUFF_SIZE 64

using namespace std;

struct Person
{
	char name[25];
	double maht;
	int inf;
	int rus;
}st;


int main()
{
	int erStat;
	Person st;

	WSADATA wsData;																		//автоматически в момент создания загружаются данные о версии сокетов, используемых ОС, а также иная связанная системная информация
																						//Функция WSAStartup инициирует использование библиотеки DLL сокетов Windows процессом
	erStat = WSAStartup(0x0202, &wsData);												// в случае успеха возвращает 0, а в случае каких-то проблем возвращает код ошибки
	if (erStat != 0)
	{
		cout << "Error WinSock version initializaion #" << WSAGetLastError() << endl;
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;


	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);								//создание и инициализация сокета
	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket #" << WSAGetLastError() << endl;
		closesocket(ClientSock);														//закрытие сокета
		WSACleanup();
		return 1;
	}
	else
		cout << "Client socket initialization is OK" << endl;


	sockaddr_in servInfo;																

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);								//inet_pton помещает в ip_to_num нужный адрес
	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}

	ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET;														//семейство адресов, тут ipv4
	servInfo.sin_addr = ip_to_num;														//тут адрес, я использую номерной, но она в книжке делает через имя localhost. Там посложнее, но один фиг в петле работаем
	servInfo.sin_port = htons(SERV_PORT);												//порт 

		
	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));				//удивительно, но это подключение к серверу			
	if (erStat != 0) 
	{	
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;


	string mst;
	short packet_size = 0;
	char buf[BUFF_SIZE] = { '\n' };
	do
	{
		cout << "Enter a query: Last name, mathematics, computer science and Russian" << endl;
		cin >> st.name >> st.maht >> st.inf >> st.rus;
		send(ClientSock, (char*)&st, sizeof(st), 0);														//отправка информации
		packet_size = recv(ClientSock, (char*)&buf, BUFF_SIZE, 0);
		buf[packet_size] = 0;
		cout << buf << endl;
		cout << "Press any button to continue or type stop for stop" << endl;
		cin >> mst;
		send(ClientSock, (char*)&mst[0], mst.size(), 0);
		cout << " " << endl;
	} while (mst != "stop");

	cout << "exit" << endl;
	closesocket(ClientSock);
	WSACleanup();
	return 0;
}