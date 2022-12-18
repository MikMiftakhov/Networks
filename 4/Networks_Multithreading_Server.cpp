#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>

#pragma comment (lib, "Ws2_32.lib")

#define SERV_PORT 1605
#define BUFF_SIZE 64

using namespace std;

const string ex = "excellent";
const string gd = "good";
const string nm = "normal";
const string bd = "bad";
const string sb = "sad";

struct Person
{
	char name[25];
	double maht;
	int inf;
	int rus;
}st;

DWORD WINAPI ConectToClient(LPVOID client_socket);						//прототип ф-ции

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


	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, 0);							//создание и инициализация сокета
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
	servInfo.sin_addr = ip_to_num;


	erStat = bind(ServSock, (sockaddr*)&servInfo, sizeof(servInfo));						//привязываем доп инфу к сокету 
	if (erStat != 0) {
		cout << "Error Socket binding to server info. Error #" << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Binding socket to Server info is OK" << endl;


	erStat = listen(ServSock, 5);																	//создаем очередь для сокета сервера, число отвечает за максимальное количество клиентов в очереди
	if (erStat != 0) {
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Listening..." << endl;


	SOCKET newClient;																				//делаем почти все тоже самое для сокета клиента
	sockaddr_in clientInfo;
	int clientInfo_size;

	ZeroMemory(&clientInfo, sizeof(clientInfo));


	string msg;

	char buf[BUFF_SIZE] = { '\n' };
	while (1)
	{
		clientInfo_size = sizeof(clientInfo);
		newClient = accept(ServSock, (sockaddr*)&clientInfo, &clientInfo_size);
		if (newClient == INVALID_SOCKET) {
			cout << "Client detected, but can't connect to a client. Error # " << WSAGetLastError() << endl;
			closesocket(ServSock);
			closesocket(newClient);
			WSACleanup();
			return 1;
		}
		else
			cout << "Connection to a client established successfully" << endl;


		char clientIP[22];
		inet_ntop(AF_INET, &clientInfo.sin_addr, clientIP, INET_ADDRSTRLEN);
		cout << "Client connected with IP address " << clientIP << endl;
		DWORD thID; // Вызов нового потока для клиента
		CreateThread(NULL, 0, ConectToClient, &newClient, NULL, &thID);						//создаем новый поток. Здесь максимально понятно расписано, что есть что https://learn.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-createthread
	}
	return 0;
}

DWORD WINAPI ConectToClient(LPVOID clientSocket)								//по факту делаем все тоже самое, что и в обычном тсп-сервере, но теперь, за счет потоков, можем работать с несколькми клиентами параллельно. Эта функция делает всю работу сервера, но в отдельном потоке
{
	SOCKET newClient;
	int len;
	newClient = ((SOCKET*)clientSocket)[0];
	char buff[BUFF_SIZE] = { '\n' };
	short packet_size = 0;
	while (1)
	{
		packet_size = recv(newClient, (char*)&st, BUFF_SIZE, 0);
		cout << st.name << " " << st.maht << " " << st.inf << " " << st.rus << endl;

		int score = (st.maht + st.inf + st.rus) / 3;
		if (score >= 90)
			send(newClient, (char*)&ex[0], ex.size(), 0);
		if (score >= 80 && score < 90)
			send(newClient, (char*)&gd[0], gd.size(), 0);
		if (score >= 60 && score < 80)
			send(newClient, (char*)&nm[0], nm.size(), 0);
		if (score >= 40 && score < 60)
			send(newClient, (char*)&bd[0], bd.size(), 0);
		if (score >= 0 && score < 40)
			send(newClient, (char*)&sb[0], sb.size(), 0);

		packet_size = recv(newClient, (char*)&buff, BUFF_SIZE, 0);
		buff[packet_size] = 0;
		string msg = (string)buff;
		if (msg == "stop") break;
	}
	cout << "Client is lost" << endl << endl;
	closesocket(newClient);
	return 0;
}