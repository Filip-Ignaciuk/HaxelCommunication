#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

#include "user.hpp"



int main()
{
	const int port = 8192;
	const int bufferSize = 200;

	

	SOCKET clientSocket;
	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;
	if(WSAStartup(version, &wsaData))
	{
		std::cout << "Winsock DLL failed to be found/loaded." << std::endl;
		std::cout << WSAGetLastError() << std::endl;
		return 0;
	}

	std::cout << "DLL has been successfully found/loaded." << std::endl;

	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		std::cout << "Error creating client socket." << std::endl;
		WSACleanup();
		return 0;
	}

	std::cout << "Client socket has been successfully created." << std::endl;

	sockaddr_in service;
	PCWSTR ip = L"127.0.0.1";
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		std::cout << "Binding failed." << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}

	std::cout << "Successfully connected to server." << std::endl;
	std::cout << "Display Name:" << std::endl;
	std::string displayName;
	std::getline(std::cin, displayName);
	std::string bufferString = "I" + displayName.size() + displayName;
	const char* buffer = bufferString.c_str();
	Sleep(2000);
	send(clientSocket, buffer, bufferSize, 0);

	User userClient("PLACEHOLDER", "999");
	while(true)
	{
		recv(clientSocket,(char*)&userClient, bufferSize, 0);
		std::cout << "Name is: " << userClient.GetDisplayName() << " and ID is: " << userClient.GetId() << std::endl;
	}



	

	

	WSACleanup();

}