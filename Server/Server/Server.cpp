#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <array>

#include "user.hpp"

const int port = 8192;
const int bufferSize = 200;

struct PositionBufferHolder
{
    std::string buffer;
    int position;
};

std::vector<std::string> texts;
int numOfUsers = 0;
User users[32];
SOCKET sockets[32];
bool finished[32];
int position = 0;

DWORD WINAPI clientInitialiseThread(LPVOID param)
{
    PositionBufferHolder* PBH = (PositionBufferHolder*)param;
	const std::string buffer = PBH->buffer;
    const int pos = PBH->position;
    delete PBH;
    std::string displayName;

    std::string displayNameLengthS = "";
    int j = 1;
    while (isdigit(buffer[j]))
    {
        displayNameLengthS.push_back(buffer[j]);
        j++;
    }
    int displayNameLength = std::stoi(displayNameLengthS) + j;

    for (int i = j; i < displayNameLength; i++)
    {
        displayName.push_back(buffer[i]);
    }

    std::cout << displayName << std::endl;


    std::cout << numOfUsers << std::endl;
    std::cout << displayName.size() << std::endl;
    User user(displayName, std::to_string(numOfUsers));
    users[position] = user;
    std::string sizeOfIdS = std::to_string(numOfUsers);
    int sizeOfId = sizeOfIdS.size();
    std::string sendBuffer = "I" + std::to_string(sizeOfId) + std::to_string(numOfUsers);
    int bytecount = send(sockets[position], sendBuffer.c_str(), bufferSize, 0);
    std::cout << bytecount << std::endl;
    finished[position] = true;
    return 0;
}

DWORD WINAPI clientMessageThread(LPVOID param)
{
    PositionBufferHolder* PBH = (PositionBufferHolder*)param;
    const std::string buffer = PBH->buffer;
    const int position = PBH->position;
    delete PBH;
    for (SOCKET socket : sockets)
    {
        int bytecount = send(socket, buffer.c_str(), bufferSize, 0);
    }
    finished[position] = true;
    return 0;
}

DWORD WINAPI clientUpdateThread(LPVOID param)
{
    PositionBufferHolder* PBH = (PositionBufferHolder*)param;
    const std::string buffer = PBH->buffer;
    const int position = PBH->position;
    delete PBH;
    std::string displayName;
    std::string id;
    if(buffer[1] == 'D')
    {
        std::cout << "Updating Display name..." << std::endl;
        std::string displayNameLengthS = "";
        int j = 2;
        while(isdigit(buffer[j]))
        {
            displayNameLengthS.push_back(buffer[j]);
            j++;
        }
        int displayNameLength = std::stoi(displayNameLengthS) + j;
        std::string newDisplayName;
        for (int i = j; i < displayNameLength; i++)
        {
            newDisplayName.push_back(buffer[i]);
        }

        users[position].SetDisplayName(newDisplayName);
        int bytecount = send(sockets[position], "U", sizeof(User), 0);

        finished[position] = true;
        return 0;

        
    }

    finished[position] = true;
    return 0;
}

DWORD WINAPI clientQuitThread(LPVOID param)
{
    const int position = (int)param;

    sockets[position] = 0;
    User user;
    users[position] = user;
    finished[position] = false;
    numOfUsers--;
    return 0;
}

DWORD WINAPI clientThreadReceive(LPVOID param)
{
    const int pos = (int)param;
    SOCKET currentSocket = sockets[pos];
    User currentUser = users[pos];
    
    char buffer[bufferSize];
    std::cout << "Detecting messages." << std::endl;
    std::cout << "Socket: " << sockets[pos] << std::endl;
    std::cout << "User: " << currentUser.GetDisplayName() << ", ID: " << currentUser.GetId() << std::endl;
    int bytecount = recv(currentSocket, buffer, bufferSize, 0);


    if (!bytecount || bytecount == SOCKET_ERROR)
    {
        std::cout << "Client is lost." << std::endl;
        sockets[pos] = 0;
        User user;
        users[pos] = user;
        finished[pos] = false;
        numOfUsers--;
        std::cout << "There is now: " << numOfUsers << " users, and " << numOfUsers << " finishes." << std::endl;
        return 0;
    }
    std::cout << "Received message. From client: " << currentSocket << std::endl;
    std::cout << "Message: " << buffer << std::endl;
    PositionBufferHolder* PBH = new PositionBufferHolder{ buffer, pos };
    if(buffer[0] == 'I')
    {

        std::cout << "Request to initialise a user. From socket: " << currentSocket << ", name:" << currentUser.GetDisplayName() << ", id: " << currentUser.GetId() << std::endl;
        DWORD threadid;
        HANDLE hdl;
        hdl = CreateThread(NULL, 0, clientInitialiseThread, PBH, 0, &threadid);
        return 0;
    }
    else if(buffer[0] == 'M')
    {
        std::cout << "Request to send a message. From client: " << currentSocket << ", name:" << currentUser.GetDisplayName() << ", id: " << currentUser.GetId() << std::endl;
        DWORD threadid;
        HANDLE hdl;
        hdl = CreateThread(NULL, 0, clientMessageThread, PBH, 0, &threadid);
        return 0;
    }
    else if(buffer[0] == 'U')
    {
        std::cout << "Request to update a user. From client: " << currentSocket << ", name:" << currentUser.GetDisplayName() << ", id: " << currentUser.GetId() << std::endl;
        DWORD threadid;
        HANDLE hdl;
        hdl = CreateThread(NULL, 0, clientUpdateThread, PBH, 0, &threadid);
        return 0;
    }
    else if(buffer[0] == 'Q')
    {
	    std::cout << "Request to quit a user. From client: " << currentSocket << ", name:" << currentUser.GetDisplayName() << ", id: " << currentUser.GetId() << std::endl;
        DWORD threadid;
        HANDLE hdl;
        hdl = CreateThread(NULL, 0, clientQuitThread, (LPVOID)pos, 0, &threadid);
        delete PBH;
        return 0;
    }

    finished[pos] = true;
    return 0;
}

bool isListenFinished = true;


DWORD WINAPI ListenThread(LPVOID param)
{

    SOCKET serverSocket = (SOCKET)param;
    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        std::cout << "Setting up listening failed." << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }

    std::cout << "Listening for clients on port: " << port << std::endl;

    SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET)
    {
        std::cout << "Accepting failed." << std::endl;
        WSACleanup();
        return 0;
    }


    std::cout << "Successfully accepted client. Socket: " << acceptSocket << std::endl;

    User user("PLACEHOLDER", "999");
    users[numOfUsers] = user;
    sockets[numOfUsers] = acceptSocket;
    finished[numOfUsers] = false;
    DWORD threadid;
    HANDLE hdl;
    std::cout << "Creating thread for socket." << std::endl;
    hdl = CreateThread(NULL, 0, clientThreadReceive, (LPVOID)numOfUsers, 0, &threadid);
    numOfUsers++;
    isListenFinished = true;
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

    std::cout << "IP: " << std::endl;
    std::string ipInput;
    std::getline(std::cin, ipInput);
    // converting ip input to const wchar_t
    std::wstring wideIpInput = std::wstring(ipInput.begin(), ipInput.end());
    PCWSTR ip = wideIpInput.c_str();

    sockaddr_in service;
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
            Sleep(250);
            hdl = CreateThread(NULL, 0, ListenThread, (LPVOID)serverSocket, 0, &threadid);

        }

        for (int i = 0; i < numOfUsers; i++)
        {
            if(finished[i])
            {
                DWORD threadid;
                HANDLE hdl;
                std::cout << i << std::endl;
                finished[i] = false;
            	hdl = CreateThread(NULL, 0, clientThreadReceive, (LPVOID)i, 0, &threadid);
            }
        }
    }
}
