#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

#define request "get/ http/1.1\r\n Host:127.0.0.1:1605\r\n\r\n"				//html запрос.

#define SERV_PORT 1605
#define max_packet_size   65535

using namespace std;

int main()
{
	int erStat;

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


	SOCKET ClientSock = socket(AF_INET, SOCK_STREAM, 0);				//создание и инициализация сокета
	if (ClientSock == INVALID_SOCKET) {
		cout << "Error initialization socket #" << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Client socket initialization is OK" << endl;

	sockaddr_in servInfo;																				//структура  с доп информацией для сокета, например ip-адресом и портом

	in_addr ip_to_num;
	erStat = inet_pton(AF_INET, "127.0.0.1", &ip_to_num);												//inet_pton помещает в ip_to_num нужный адрес	//работаю через внутреннюю петлю, ибо с другими не работает) Нашему было абслоютно похуй, мб вашей нет
	if (erStat <= 0) {
		cout << "Error in IP translation to special numeric format" << endl;
		return 1;
	}

	ZeroMemory(&servInfo, sizeof(servInfo));
	servInfo.sin_family = AF_INET;
	servInfo.sin_addr = ip_to_num;
	servInfo.sin_port = htons(1605);

	erStat = connect(ClientSock, (sockaddr*)&servInfo, sizeof(servInfo));								//привязываем доп инфу к сокету 
	if (erStat != 0) {
		cout << "Connection to Server is FAILED. Error # " << WSAGetLastError() << endl;
		closesocket(ClientSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Connection established SUCCESSFULLY. Ready to send a message to Server" << endl;

		
	erStat = send(ClientSock, (char*)&request, sizeof(request), 0);										//отправка запроса серверу
	if (SOCKET_ERROR == erStat)
	{
		cout << "Send errorr # " << WSAGetLastError() << endl;
		return -1;
	}
	else
		cout << "Sending successful" << endl;

	char buff[max_packet_size];
	int len = 0;
	do
	{
		len = recv(ClientSock, (char*)&buff, max_packet_size, 0);										//Получаем инфу
		if (SOCKET_ERROR == len)
		{
			cout << "Receive error # " << WSAGetLastError() << endl;
			return -1;
		}
		else
			cout << "Receipt Successfully" << endl;

		for (int i = 0; i < len; i++)
			cout << buff[i];
	} while (len != 0);																						//получаем данные по частям, пока не len != 0.
	closesocket(ClientSock);
	cin.get();
	return 1;
}