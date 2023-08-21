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

struct SocketBufferPositionHolder
{
    SOCKET socket;
    std::string buffer;
    int pos;

};

std::vector<std::string> texts;
std::vector<SocketUserHolder> users;
std::vector<bool> finished;
std::vector<bool> entered;
int position = 0;

DWORD WINAPI clientInitialiseThread(LPVOID param)
{
    SocketBufferPositionHolder* SBPH = (SocketBufferPositionHolder*)param;
    std::string displayName;
    std::string id;

    std::string displayNameLengthS = "";
    int j = 1;
    while (isdigit(SBPH->buffer[j]))
    {
        displayNameLengthS.push_back(SBPH->buffer[j]);
        j++;
    }
    int displayNameLength = std::stoi(displayNameLengthS) + j;

    for (int i = j; i < displayNameLength; i++)
    {
        displayName.push_back(SBPH->buffer[i]);
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
    users[SBPH->pos].user = user;
    std::cout << "Sending user object" << std::endl;
    std::cout << "DisplayName = " << user.GetDisplayName() << std::endl;
    std::cout << "ID = " << user.GetId() << std::endl;
    int bytecount = send(SBPH->socket, (char*)&user, sizeof(User), 0);

    
    finished[SBPH->pos] = true;
    delete SBPH;
    return 0;
}

DWORD WINAPI clientMessageThread(LPVOID param)
{
    SocketBufferPositionHolder* SBPH = (SocketBufferPositionHolder*)param;
    for (SocketUserHolder SUH : users)
    {
        int bytecount = send(SUH.socket, SBPH->buffer.c_str(), bufferSize, 0);
    }

    
    finished[SBPH->pos] = true;
    delete SBPH;
    return 0;
}

DWORD WINAPI clientUpdateThread(LPVOID param)
{
    SocketBufferPositionHolder* SBPH = (SocketBufferPositionHolder*)param;
    std::string displayName;
    std::string id;
    if(SBPH->buffer[1] == 'D')
    {
        std::cout << "Updating Display name..." << std::endl;
        std::string displayNameLengthS = "";
        int j = 2;
        while(isdigit(SBPH->buffer[j]))
        {
            displayNameLengthS.push_back(SBPH->buffer[j]);
            j++;
        }
        int displayNameLength = std::stoi(displayNameLengthS) + j + 1;
        std::string newDisplayName;
        for (int i = j + 1; i < displayNameLength; i++)
        {
            newDisplayName.push_back(SBPH->buffer[i]);
        }

        for (int i = 0; i < users.size(); i++)
        {
            if(users[i].socket == SBPH->socket)
            {
                users[i].user.SetDisplayName(newDisplayName);
                int bytecount = send(SBPH->socket, (char*)&users[i].user, sizeof(User), 0);
            }
        }

        finished[SBPH->pos] = true;
        delete SBPH;
        return 0;

        
    }

    finished[SBPH->pos] = true;
    delete SBPH;
    return 0;
}


DWORD WINAPI clientThreadReceive(LPVOID param)
{

    SocketPositionHolder* SPH = (SocketPositionHolder*)param;
    
    char buffer[bufferSize];
    std::cout << "Detecting messages." << std::endl;
    std::cout << "Socket: " << SPH->socket << std::endl;
    int bytecount = recv(SPH->socket, buffer, bufferSize, 0);
    
    if (!bytecount)
    {
        users.erase(users.begin() + SPH->pos);
        finished.erase(finished.begin() + SPH->pos);
        delete SPH;
        return 0;
    }
    std::cout << "Received message. From client: " << SPH->socket << std::endl;
    std::cout << "Message: " << buffer << std::endl;
    if(buffer[0] == 'I')
    {
        std::cout << "Request to initialise a user. From socket: " << SPH->socket << ", name:" << users[SPH->pos].user.GetDisplayName() << ", id: " << users[SPH->pos].user.GetId() << std::endl;
        DWORD threadid;
        HANDLE hdl;
        SocketBufferPositionHolder* SBPH = new SocketBufferPositionHolder{ SPH->socket, buffer, SPH->pos };
        hdl = CreateThread(NULL, 0, clientInitialiseThread, SBPH, 0, &threadid);
        delete SPH;
        return 0;
    }
    else if(buffer[0] == 'M')
    {
        std::cout << "Request to send a message. From client: " << SPH->socket << ", name:" << users[SPH->pos].user.GetDisplayName() << ", id: " << users[SPH->pos].user.GetId() << std::endl;
        DWORD threadid;
        HANDLE hdl;
        SocketBufferPositionHolder* SBPH = new SocketBufferPositionHolder{ SPH->socket, buffer, SPH->pos };
        hdl = CreateThread(NULL, 0, clientMessageThread, SBPH, 0, &threadid);
        delete SPH;
        return 0;
    }
    else if(buffer[0] == 'U')
    {
        std::cout << "Request to update a user. From client: " << SPH->socket << ", name:" << users[SPH->pos].user.GetDisplayName() << ", id: " << users[SPH->pos].user.GetId() << std::endl;
        DWORD threadid;
        HANDLE hdl;
        SocketBufferPositionHolder* SBPH = new SocketBufferPositionHolder{ SPH->socket, buffer, SPH->pos };
        hdl = CreateThread(NULL, 0, clientUpdateThread, SBPH, 0, &threadid);
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
    SocketUserHolder SUH = { acceptSocket,user };
    users.emplace_back(SUH);

    DWORD threadid;
    HANDLE hdl;
    SocketPositionHolder* SPH = new SocketPositionHolder{ acceptSocket, position };
    std::cout << "Creating thread for socket." << std::endl;
    Sleep(250);
    hdl = CreateThread(NULL, 0, clientThreadReceive, SPH, 0, &threadid);
    
	finished.emplace_back(false);
    position++;
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

        for (int i = 0; i < finished.size(); i++)
        {
            if(finished[i])
            {
                DWORD threadid;
                HANDLE hdl;
                std::cout << i << std::endl;
                SocketPositionHolder* SPH = new SocketPositionHolder{ users[i].socket, i };
                finished[i] = false;
                Sleep(250);
                hdl = CreateThread(NULL, 0, clientThreadReceive, SPH, 0, &threadid);
            }
        }
    }
}
