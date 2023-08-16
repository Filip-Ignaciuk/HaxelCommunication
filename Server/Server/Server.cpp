#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>

int main()
{
    SOCKET serverSocket = INVALID_SOCKET;
	const int port = 8192;
    WSADATA wsaData;
    WORD version = MAKEWORD(2, 2);
    if (WSAStartup(version, &wsaData))
    {
        std::cout << "Winsock DLL failed to be found/loaded." << std::endl;
        std::cout << WSAGetLastError() << std::endl;
        return 0;
    }

    std::cout << "DLL has been successfully found/loaded." << std::endl;

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cout << "Error creating server socket" << std::endl;
        WSACleanup();
        return 0;
    }

    std::cout << "Server socket has been successfully created." << std::endl;

    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(port);
    service.sin_addr.S_un.S_addr = INADDR_ANY;
    
    if(bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
    {
        std::cout << "Binding failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }

    std::cout << "Server socket bind has been successfully done." << std::endl;


    if(listen(serverSocket, 1) == SOCKET_ERROR)
    {
        std::cout << "Setting up listening failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }

    std::cout << "Successfully listening for clients" << std::endl;

    SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET)
    {
        std::cout << "Accepting failed." << std::endl;
    	WSACleanup();
        return 0;
    }

    std::cout << "Successfully accepted socket." << std::endl;
    system("pause");
    WSACleanup();
}