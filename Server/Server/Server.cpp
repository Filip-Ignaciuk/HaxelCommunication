#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>

#include "user.hpp"
#include "gui.h"	
#include "ImGui/imgui.h"

const int port = 8192;
const int bufferSize = 200;

std::vector<std::string> allServerText;

std::string currentError;

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

DWORD WINAPI ClientTableThread(LPVOID param)
{
    const int position = int(param);
    User user = users[position];

    std::string userDisplayName = user.GetDisplayName();
    const int sizeOfDisplayName = userDisplayName.size();

    std::string userId = user.GetId();
    const int sizeOfId = userId.size();

    std::string sendBuffer = "T" + std::to_string(sizeOfId) + userId + std::to_string(sizeOfDisplayName) + userDisplayName;
    int bytecount;
    for (SOCKET socket : sockets)
    {
        if(socket)
        {
            bytecount = send(socket, sendBuffer.c_str(), bufferSize, 0);
        }
        
    }

    return 0;
}

DWORD WINAPI ClientAllTableThread(LPVOID param)
{
    const int socket = sockets[(int)param];
    int bytecount;
	for (User user : users)
	{
        if(user.GetDisplayName() != "" && user.GetId() != "")
        {
            std::string userDisplayName = user.GetDisplayName();
            const int sizeOfDisplayName = userDisplayName.size();

            std::string userId = user.GetId();
            const int sizeOfId = userId.size();

            std::string sendBuffer = "T" + std::to_string(sizeOfId) + userId + std::to_string(sizeOfDisplayName) + userDisplayName;
            
            bytecount = send(socket, sendBuffer.c_str(), bufferSize, 0);
        }
        
	}
    

    return 0;
}


void UpdateTable(int _position)
{
    DWORD threadid;
    HANDLE hdl;
    hdl = CreateThread(NULL, 0, ClientTableThread, (LPVOID)_position, 0, &threadid);
}

void UpdateThisTable(int _position)
{
    DWORD threadid;
    HANDLE hdl;
    hdl = CreateThread(NULL, 0, ClientAllTableThread, (LPVOID)_position, 0, &threadid);
}


DWORD WINAPI ClientInitialiseThread(LPVOID param)
{
    PositionBufferHolder* PBH = (PositionBufferHolder*)param;
	const std::string buffer = PBH->buffer;
    const int position = PBH->position;
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
    // To Do
    //if(bufferSize < sizeof(buffer))
    //{
	//    
    //}
    //else
    //{
	//    
    //}
    int bytecount = send(sockets[position], sendBuffer.c_str(), bufferSize, 0);
    UpdateTable(position);
    UpdateThisTable(position);
    
    finished[position] = true;
    return 0;
}



DWORD WINAPI ClientMessageThread(LPVOID param)
{
    PositionBufferHolder* PBH = (PositionBufferHolder*)param;
    const std::string buffer = PBH->buffer;
    const int position = PBH->position;
    delete PBH;
    for (SOCKET socket : sockets)
    {
        if(socket)
        {
            int bytecount = send(socket, buffer.c_str(), bufferSize, 0);
        }
        
    }
    finished[position] = true;
    return 0;
}

DWORD WINAPI ClientUpdateThread(LPVOID param)
{
    PositionBufferHolder* PBH = (PositionBufferHolder*)param;
    const std::string buffer = PBH->buffer;
    const int position = PBH->position;
    delete PBH;
    std::string displayName;
    std::string id;
    if(buffer[1] == 'D')
    {
        allServerText.emplace_back("Updating Display name...");
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
        int bytecount = send(sockets[position], "U", bufferSize, 0);
        UpdateTable(position);
        finished[position] = true;
        return 0;

        
    }

    finished[position] = true;
    return 0;
}

DWORD WINAPI ClientQuitThread(LPVOID param)
{
    const int position = (int)param;

    sockets[position] = 0;
    User user;
    users[position] = user;
    finished[position] = false;
    numOfUsers--;
    UpdateTable(position);
    return 0;
}

DWORD WINAPI ClientThreadReceive(LPVOID param)
{
    const int pos = (int)param;
    SOCKET currentSocket = sockets[pos];
    User currentUser = users[pos];
    
    char buffer[bufferSize];
    allServerText.emplace_back("Detecting messages.");
    allServerText.emplace_back("Socket: " + std::to_string(sockets[pos]));
    allServerText.emplace_back("User: " + currentUser.GetDisplayName() + ", ID: " + currentUser.GetId());
    int bytecount = recv(currentSocket, buffer, bufferSize, 0);


    if (!bytecount || bytecount == SOCKET_ERROR)
    {
        allServerText.emplace_back("Client is lost.");
        sockets[pos] = 0;
        User user;
        users[pos] = user;
        finished[pos] = false;
        numOfUsers--;
        allServerText.emplace_back("There is now: " + std::to_string(numOfUsers) + " users.");
        return 0;
    }

    allServerText.emplace_back("Received message. From client: " + std::to_string(currentSocket));
    PositionBufferHolder* PBH = new PositionBufferHolder{ buffer, pos };
    if(buffer[0] == 'I')
    {
        allServerText.emplace_back("Request to initialise a user. From socket: " + std::to_string(currentSocket) + ", name: " + currentUser.GetDisplayName() + ", id: " + currentUser.GetId());
        DWORD threadid;
        HANDLE hdl;
        hdl = CreateThread(NULL, 0, ClientInitialiseThread, PBH, 0, &threadid);
        return 0;
    }
    else if(buffer[0] == 'M')
    {
        allServerText.emplace_back("Request to send a message. From client: " + std::to_string(currentSocket) + ", name: " + currentUser.GetDisplayName() + ", id: " + currentUser.GetId());

        DWORD threadid;
        HANDLE hdl;
        hdl = CreateThread(NULL, 0, ClientMessageThread, PBH, 0, &threadid);
        return 0;
    }
    else if(buffer[0] == 'U')
    {
        allServerText.emplace_back("Request to update a user. From client: " + std::to_string(currentSocket) + ", name: " + currentUser.GetDisplayName() + ", id: " + currentUser.GetId());
        DWORD threadid;
        HANDLE hdl;
        hdl = CreateThread(NULL, 0, ClientUpdateThread, PBH, 0, &threadid);
        return 0;
    }
    else if(buffer[0] == 'Q')
    {
        allServerText.emplace_back("Request to quit a user. From client: " + std::to_string(currentSocket) + ", name: " + currentUser.GetDisplayName() + ", id: " + currentUser.GetId());
        DWORD threadid;
        HANDLE hdl;
        hdl = CreateThread(NULL, 0, ClientQuitThread, (LPVOID)pos, 0, &threadid);
        delete PBH;
        allServerText.emplace_back("There is now: " + std::to_string(numOfUsers) + " users.");
        return 0;
    }

    finished[pos] = true;
    return 0;
}

struct DomainHolder
{
    std::string domainName;
    std::string domainPassword;
    std::string domainAdminPassword;
};

DWORD WINAPI RegisterServerDomain(LPVOID param)
{
    DomainHolder* DH = (DomainHolder*)param;
    std::string domainName = DH->domainName;
    std::string domainPassword = DH->domainPassword;
    std::string domainAdminPassword = DH->domainAdminPassword;
    delete DH;

    SOCKET domainSocket = INVALID_SOCKET;

    domainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (domainSocket == INVALID_SOCKET)
    {
        currentError = "Error creating domain socket.";
        WSACleanup();
        return 0;
    }

    allServerText.emplace_back("Socket for domain server has been successfully created.");

    PCWSTR ip = L"127.0.0.1";
    sockaddr_in service;
    service.sin_family = AF_INET;
    service.sin_port = htons(4096);
    InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

    if (connect(domainSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
    {
        allServerText.emplace_back("Connecting to the domain server was unsuccessful.");
    }
    else
    {
        std::string buffer = "R" + std::to_string(domainName.size()) + "B" + std::to_string(domainPassword.size()) + "B" + std::to_string(domainAdminPassword.size()) + "B" + domainName + domainPassword + domainAdminPassword;
        char bufferResponse[bufferSize];
    	send(domainSocket, buffer.c_str(), bufferSize, 0);
        recv(domainSocket, bufferResponse, bufferSize, 0);
        int result = std::stoi(bufferResponse);
        if(!result)
        {
            allServerText.emplace_back("Registering domain was successful.");
        }
        else if (result == 1)
        {
            allServerText.emplace_back("Registering domain was unsuccessful, domain already exists!");
        }
        else if (result == 2)
        {
            allServerText.emplace_back("Registering domain was unsuccessful, domain, password or the admin password was empty.");
        }

    }
}

bool isListenFinished = true;


DWORD WINAPI ListenThread(LPVOID param)
{

    SOCKET serverSocket = (SOCKET)param;
    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        currentError = "Setting up listening failed.";
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    allServerText.emplace_back("Listening for clients on port: " + std::to_string(port));
    SOCKET acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET)
    {
        currentError = "Accepting failed.";
        WSACleanup();
        return 0;
    }


    allServerText.emplace_back("Successfully accepted client. Socket: " + std::to_string(acceptSocket));

    bool isConnected = false;

    for(size_t i = 0; i < 32; i++)
    {
        if(!sockets[i])
        {
            User user("PLACEHOLDER", "999");
            users[i] = user;
            sockets[i] = acceptSocket;
            finished[i] = false;
            DWORD threadid;
            HANDLE hdl;
            allServerText.emplace_back("Creating thread for socket.");
            hdl = CreateThread(NULL, 0, ClientThreadReceive, (LPVOID)numOfUsers, 0, &threadid);
            numOfUsers++;
            isListenFinished = true;
            isConnected = true;
            break;
        }
        
    }

    if(!isConnected)
    {
        allServerText.emplace_back("Max clients reached!, Declining socket.");
        int bytecount = send(acceptSocket, "EM", sizeof("EM"), 0);
    }
    
}

int __stdcall wWinMain(HINSTANCE _instace, HINSTANCE _previousInstance, PWSTR _arguments, int commandShow)
{
    gui::CreateHWindow("Haxel Communication Server", "haxelClass");
    gui::CreateDevice();
    gui::CreateImGui();



    SOCKET serverSocket = INVALID_SOCKET;
    WSADATA wsaData;
    WORD version = MAKEWORD(2, 2);
    if (WSAStartup(version, &wsaData))
    {
        currentError = "Winsock DLL failed to be found/loaded.";
        std::cout << WSAGetLastError() << std::endl;
        return 0;
    }

    allServerText.emplace_back("DLL has been successfully found/loaded.");

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        currentError = "Error creating server socket.";
        WSACleanup();
        return 0;
    }

    allServerText.emplace_back("Server socket has been successfully created.");

    char charIp[bufferSize] = "";
    char charPort[bufferSize] = "";

    char charDomainName[bufferSize] = "";
    char charPassword[bufferSize] = "";
    char charAdminPassword[bufferSize] = "";

    bool wantsToExit = false;
    bool isServerInitialised = false;
    bool isPortOrIPValid = true;

    int previousSize = 0;

    while (gui::exit && !wantsToExit)
    {
        isPortOrIPValid = true;
        gui::BeginRender();
        gui::Render();
        ImGui::SeparatorText("Current Error");
        ImGui::Text(currentError.c_str());

        
        ImGui::SeparatorText("Server Configuration");
        if (ImGui::InputText("IP", charIp, IM_ARRAYSIZE(charIp), ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::InputText("Port", charPort, IM_ARRAYSIZE(charPort), ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Create Server"))
        {
            std::string IP = charIp;
            int dotCount = 0;
            for (unsigned char i = 0; i < IP.size(); i++)
            {
				if (IP[i] == '.')
				{
					dotCount++;
				}
                else if (!isdigit(IP[i]))
                {
                    isPortOrIPValid = false;
                }
                
            }

            std::string port = charPort;
            for (unsigned char j = 0; j < port.size(); j++)
            {
                if (!isdigit(port[j]))
                {
                    isPortOrIPValid = false;
                    break;
                }

            }
            if(dotCount != 3)
            {
                isPortOrIPValid = false;
            }
            

            if(isPortOrIPValid)
            {
                
                // converting ip input to const wchar_t
                std::wstring wideIpInput = std::wstring(IP.begin(), IP.end());
                PCWSTR ip = wideIpInput.c_str();
                sockaddr_in service;
                service.sin_family = AF_INET;
                service.sin_port = htons(std::stoi(port));
                InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);
                if (bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
                {
                    currentError = "Binding failed, perhaps an invalid or non-existent ip";
                }
                else
                {
                    isServerInitialised = true;
                    allServerText.emplace_back("Server socket bind has been successfully done.");
                }
                
            }
            else
            {
                currentError = "Ip or port format inputted is invalid.";
            }
            

            
        }

        if (ImGui::InputText("Name", charDomainName, IM_ARRAYSIZE(charDomainName), ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::InputText("Password", charPassword, IM_ARRAYSIZE(charPassword), ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::InputText("Admin Password", charAdminPassword, IM_ARRAYSIZE(charAdminPassword), ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("List server domain"))
        {
            DomainHolder* DH = new DomainHolder{ charDomainName, charPassword, charAdminPassword };
            DWORD threadid;
            HANDLE hdl;
            hdl = CreateThread(NULL, 0, RegisterServerDomain, (LPVOID)DH, 0, &threadid);

        }

        if (ImGui::Button("Exit"))
        {
            wantsToExit = true;
        }

        if(isServerInitialised)
        {
            if (isListenFinished)
            {
                isListenFinished = false;
                DWORD threadid;
                HANDLE hdl;
                Sleep(250);
                hdl = CreateThread(NULL, 0, ListenThread, (LPVOID)serverSocket, 0, &threadid);

            }

            for (int i = 0; i < numOfUsers; i++)
            {
                if (finished[i])
                {
                    DWORD threadid;
                    HANDLE hdl;
                    std::cout << i << std::endl;
                    finished[i] = false;
                    hdl = CreateThread(NULL, 0, ClientThreadReceive, (LPVOID)i, 0, &threadid);
                }
            }
        }

        ImGui::SeparatorText("Server information");
        std::string infoUsers = "Number of connected users: " + std::to_string(numOfUsers);
        ImGui::Text(infoUsers.c_str());

        ImGui::SeparatorText("Activity");
        ImGui::BeginChild("Scrolling");
        const int currentSize = allServerText.size();

        if (previousSize == currentSize)
        {
            for (int i = 0; i < currentSize; i++)
            {
                ImGui::TextWrapped(allServerText[i].c_str());
            }
        }
        else
        {
            for (int i = 0; i < currentSize; i++)
            {
                // Wont work when a new message comes.
                ImGui::TextWrapped(allServerText[i].c_str());
                previousSize = currentSize;
                ImGui::SetScrollHereY(1.0f);
            }
        }

        

        
        ImGui::EndChild();
        ImGui::End();
        gui::EndRender();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    for (SOCKET socket : sockets)
    {
        int byteCount = send(socket, "QS", bufferSize, 0);
    }



    WSACleanup();
    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();
    return EXIT_SUCCESS;
}
