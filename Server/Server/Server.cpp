#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>

#include "user.hpp"


std::vector<std::string> texts;


DWORD WINAPI clientThreadSendUserText(LPVOID param)
{
    SOCKET clientSocket = (SOCKET)param;
    char buffer[200];
    int bytecount = send(clientSocket, buffer, 200, 0);

    

}

DWORD WINAPI clientThreadReceive(LPVOID param)
{
    SOCKET clientSocket = (SOCKET)param;
    char buffer[200];
    int bytecount = recv(clientSocket, buffer, 200, 0);

    if (buffer[0] == 'U' && buffer[1] == 'T')
    {
        //UT[User]Hello!
        //In square brackets is the user class
        clientThreadSendUserText(buffer);
        buffer[0] = (char)"";
        buffer[1] = (char)"";
    }

}

DWORD WINAPI clientThread(LPVOID param)
{
    SOCKET clientSocket = (SOCKET)param;

    recv(clientSocket, )
}

int main()
{
    const int port = 8192;




    SOCKET serverSocket = INVALID_SOCKET;
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
        std::cout << "Error creating server socket." << std::endl;
        WSACleanup();
        return 0;
    }

    std::cout << "Server socket has been successfully created." << std::endl;

    sockaddr_in service;
    PCWSTR ip = L"127.0.0.1";
    service.sin_family = AF_INET;
    service.sin_port = htons(port);
    InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);
    
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

    DWORD threadid;
    HANDLE hdl;
    hdl = CreateThread(NULL, 0, clientThread, (LPVOID)acceptSocket, 0, &threadid);

    WSACleanup();
}
