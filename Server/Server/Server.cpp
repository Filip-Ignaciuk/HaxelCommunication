#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>

#include "user.hpp"

const int port = 8192;
const int bufferSize = 200;

std::vector<std::string> texts;
std::vector<User> users;
std::vector<bool> finished;

struct SocketUserHolder
{
    SOCKET socket;
    User user;
};

struct SocketBufferHolder
{
    SOCKET socket;
    char buffer[bufferSize];
};

DWORD WINAPI clientThreadI(LPVOID param)
{
    SocketBufferHolder* SBH = (SocketBufferHolder*)param;
    std::string displayName;
    std::string id;

    int displayNameLength = SBH->buffer[1];

    for (size_t i = 2; i < displayNameLength; i++)
    {
        displayName.push_back(SBH->buffer[i]);
    }

    int numOfUsers = users.size();
    if (numOfUsers < 1000)
    {
        if (numOfUsers < 10)
        {
            id = numOfUsers + "00";
        }
        if (numOfUsers < 100)
        {
            id = numOfUsers + "0";
        }
        else
        {
            id = numOfUsers;
        }
    }
    
    User user(displayName, id);
    char* buffer = 'I' + (char*)&user;
    int bytecount = send(SBH->socket, (char*)&buffer, bufferSize, 0);

    
    return 0;
}

DWORD WINAPI clientThreadU(LPVOID param)
{
    SOCKET clientSocket = (SOCKET)param;
    char buffer[bufferSize];
    int bytecount = send(clientSocket, buffer, bufferSize, 0);


    return 0;
}


DWORD WINAPI clientThreadReceive(LPVOID param)
{
    SOCKET clientSocket = (SOCKET)param;
    char buffer[bufferSize];
    int bytecount = recv(clientSocket, buffer, bufferSize, 0);

    if(buffer[0] == 'I')
    {
        DWORD threadid;
        HANDLE hdl;
        SocketBufferHolder SBH = { clientSocket, (char)buffer };
        hdl = CreateThread(NULL, 0, clientThreadI, param, 0, &threadid);
    }
    else if(buffer[0] == 'U')
    {
	    
    }

    return 0;
}

//DWORD WINAPI clientThread(LPVOID param)
//{
//    SOCKET clientSocket = (SOCKET)param;
//    DWORD threadid;
//    HANDLE hdl;
//    hdl = CreateThread(NULL, 0, clientThreadReceive, param, 0, &threadid);
//
//
//}

int main()
{
    




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
    hdl = CreateThread(NULL, 0, clientThreadReceive, (LPVOID)acceptSocket, 0, &threadid);
    finished.emplace_back(false);
    while(true)
    {
	    
    }
}
