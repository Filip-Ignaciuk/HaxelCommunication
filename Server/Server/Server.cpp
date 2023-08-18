#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>

#include "user.hpp"

const int port = 8192;
const int bufferSize = 200;

struct SocketPositionHolder
{
    SOCKET socket;
    int pos;
};

struct SocketUserHolder
{
    SOCKET socket;
    User user;
};

struct SocketBufferHolder
{
    SOCKET socket;
    std::string buffer;
};


std::vector<std::string> texts;
std::vector<SocketUserHolder> users;
std::vector<bool> finished;
int pos = 0;

DWORD WINAPI clientThreadI(LPVOID param)
{
    SocketBufferHolder* SBH = (SocketBufferHolder*)param;
    std::string displayName;
    std::string id;

    std::string displayNameLengthS = "0";
    displayNameLengthS[0] = SBH->buffer[1];
    int displayNameLength = std::stoi(displayNameLengthS) + 2;

    for (int i = 2; i < displayNameLength; i++)
    {
        displayName.push_back(SBH->buffer[i]);
    }

    std::cout << displayName << std::endl;
    int numOfUsers = users.size();
    
    if (numOfUsers < 1000)
    {
        if (numOfUsers < 10)
        {
            id = std::to_string(numOfUsers) + "00";
        }
        else if (numOfUsers < 100)
        {
            id = std::to_string(numOfUsers) + "0";
        }
        else
        {
            id = std::to_string(numOfUsers);
        }
    }

    std::cout << id << std::endl;
    
    User user(displayName, id);
    int pos;
    for (size_t i = 0; i < users.size(); i++)
    {
        if(users[i].socket == SBH->socket)
        {
            users[i].user = user;
            pos = i;
        }
    }
    std::cout << "Sending user object" << std::endl;
    std::cout << "DisplayName = " << user.GetDisplayName() << std::endl;
    std::cout << "ID = " << user.GetId() << std::endl;
    int bytecount = send(SBH->socket, (char*)&user, sizeof(User), 0);

    delete SBH;
    finished[pos] = true;
    
    return 0;
}

DWORD WINAPI clientThreadU(LPVOID param)
{
    SocketBufferHolder* SBH = (SocketBufferHolder*)param;
    std::string displayName;
    std::string id;

    if(SBH->buffer[1] == 'D')
    {
        std::string displayNameLengthS = "0";
        displayNameLengthS[0] = SBH->buffer[2];
        int displayNameLength = std::stoi(displayNameLengthS) + 3;
        std::string newDisplayName;
        for (int i = 3; i < displayNameLength; i++)
        {
            newDisplayName.push_back(SBH->buffer[i]);
        }

        for (int i = 0; i < users.size(); i++)
        {
            if(users[i].socket == SBH->socket)
            {
                users[i].user.SetDisplayName(newDisplayName);
                int bytecount = send(SBH->socket, (char*)&users[i].user, sizeof(User), 0);
            }
        }

        
    }

    delete param;
    return 0;
}


DWORD WINAPI clientThreadReceive(LPVOID param)
{
    Sleep(2000);
    SocketPositionHolder* SPH = (SocketPositionHolder*)param;

    char buffer[bufferSize];
    std::cout << "Receiving message..." << std::endl;
    int bytecount = recv(SPH->socket, buffer, bufferSize, 0);
    std::cout << "Checking message." << std::endl;
    std::cout << "Message: " << buffer << std::endl;
    if(buffer[0] == 'I')
    {
        std::cout << "Initialising user" << std::endl;
        DWORD threadid;
        HANDLE hdl;
        SocketBufferHolder* SBH = new SocketBufferHolder{ SPH->socket, buffer };
        hdl = CreateThread(NULL, 0, clientThreadI, SBH, 0, &threadid);
        delete SPH;
        return 0;
    }
    else if(buffer[0] == 'U')
    {
        std::cout << "Updating user" << std::endl;
        DWORD threadid;
        HANDLE hdl;
        SocketBufferHolder* SBH = new SocketBufferHolder{ SPH->socket, buffer };
        hdl = CreateThread(NULL, 0, clientThreadU, SBH, 0, &threadid);
        delete SPH;
        return 0;
    }

    finished[SPH->pos] = true;
    delete SPH;
    return 0;
}

bool isListenFinished = true;


DWORD WINAPI ListenThread(LPVOID param)
{
    std::cout << "Listening..." << std::endl;
    SOCKET serverSocket = (SOCKET)param;
    if (listen(serverSocket, 1) == SOCKET_ERROR)
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

    User user("PLACEHOLDER", "999");
    SocketUserHolder SUH = { acceptSocket,user };
    users.emplace_back(SUH);

    DWORD threadid;
    HANDLE hdl;
    SocketPositionHolder* SPH = new SocketPositionHolder{ acceptSocket, pos };
    std::cout << "Creating thread for socket." << std::endl;
    hdl = CreateThread(NULL, 0, clientThreadReceive, SPH, 0, &threadid);
    
	finished.emplace_back(false);
    pos++;

}

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


   
    while(true)
    {
        if(isListenFinished)
        {
            isListenFinished = false;
            DWORD threadid;
            HANDLE hdl;
            hdl = CreateThread(NULL, 0, ListenThread, (LPVOID)serverSocket, 0, &threadid);

        }

        for (int i = 0; i < finished.size(); i++)
        {
            if(finished[i])
            {
                DWORD threadid;
                HANDLE hdl;
                SocketPositionHolder SPH = { users[i].socket, i };
                hdl = CreateThread(NULL, 0, clientThreadReceive, &SPH, 0, &threadid);
            }
        }
    }
}
