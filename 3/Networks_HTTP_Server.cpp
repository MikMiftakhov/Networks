#include <iostream>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <sstream>
#include <string>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int main()
{
	int erStat;																								
	WSADATA wsData;											
															
	erStat = WSAStartup(0x0202, &wsData);					
	if (erStat != 0)
	{
		cout << "Error WinSock version initializaion #" << WSAGetLastError() << endl;
		return 1;
	}
	else
		cout << "WinSock initialization is OK" << endl;

	struct addrinfo* servInfo = NULL;												//более удобная структура для добавления информации сокету
	struct addrinfo hints;															// структура для IP-адреса  сервера
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;	
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	erStat = getaddrinfo("127.0.0.1", "1605", &hints, &servInfo);					//не ебу как работаает, по идее тоже привязка к сокету
	if (erStat != 0)
	{
		cout << "Getaddrinfo failed: " << erStat << "\n";
		WSACleanup();
	}
	else
		cout << "Getaddrinfo is OK" << endl;

	int ServSock = socket(servInfo->ai_family, servInfo->ai_socktype, servInfo->ai_protocol);				//создание и инициализация сокета
	if (ServSock == INVALID_SOCKET) {
		cout << "Error initialization socket #" << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Server socket initialization is OK" << endl;

	erStat = bind(ServSock, servInfo->ai_addr, (int)servInfo->ai_addrlen);						//привязываем доп инфу к сокету 
	if (erStat != 0) {
		cout << "Error Socket binding to server info. Error #" << WSAGetLastError() << endl;
		freeaddrinfo(servInfo);
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Binding socket to Server info is OK" << endl;

	erStat = listen(ServSock, 3);
	if (erStat != 0) {
		cout << "Can't start to listen to. Error # " << WSAGetLastError() << endl;
		closesocket(ServSock);
		WSACleanup();
		return 1;
	}
	else
		cout << "Listening..." << endl;

	const int max_client_buffer_size = 1024;
	char buf[max_client_buffer_size];
	int ClientSock = INVALID_SOCKET;

	while (1)
	{
		ClientSock = accept(ServSock, NULL, NULL);
		if (ClientSock == INVALID_SOCKET)
		{
			cout << "Accept failed: " << WSAGetLastError() << endl;
			closesocket(ServSock);
			WSACleanup();
			return 1;
		}
		else
			cout << "Accept" << endl;

		erStat = recv(ClientSock, buf, max_client_buffer_size, 0);
		std::stringstream response;
		std::stringstream response_body;
		if (erStat == SOCKET_ERROR)
		{
			cout << "Recv failed: " << erStat << endl;
			closesocket(ClientSock);
		}
		else if (erStat == 0)
			cout << "Connection closed..."  << endl;
		else if (erStat > 0)
		{
			buf[erStat] = '\0';
			response_body << "<title>Test C++ HTTP Server</title>\n"			// формируем тело ответа (HTML)
				<< "<h1>Test page</h1>\n"
				<< "<p>This is body of the test page...</p>\n"
				<< "<h2>Request headers</h2>\n"
				<< "<pre>" << buf << "</pre>\n"
				<< "<em><small>Test C++ Http Server</small></em>\n";

			// Формируем весь ответ вместе с заголовками
			response << "HTTP/1.1 200 OK\r\n" << "Version: HTTP/1.1\r\n" << "Content-Type: text/html; charset=utf-8\r\n" << "Content-Length: " << response_body.str().length()
				<< "\r\n\r\n" << response_body.str();

			// Отправляем ответ клиенту 
			erStat = send(ClientSock, response.str().c_str(),
					response.str().length(), 0);
			if (erStat == SOCKET_ERROR)
				cerr << "Send failed:" << WSAGetLastError() << endl; // произошла ошибка при отправке данных
			else
				cout << "Send"<< endl;
			// Закрываем соединение к клиентом
			closesocket(ClientSock);
		}
	}
	closesocket(ServSock);
	freeaddrinfo(servInfo);
	WSACleanup();
	return 0;
}

//дальше либо через браузер открываем 127.0.0.1:1605, либо через telnet   open 127.0.0.1 1605