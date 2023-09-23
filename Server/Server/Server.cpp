#include "stdafx.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>

#include "BufferStandard.hpp"
#include "user.hpp"
#include "gui.h"	
#include "ImGui/imgui.h"

std::vector<std::string> allServerText;

std::string currentError;
std::string currentPort;
std::string currentIp;

struct PositionBufferHolder
{
    std::string buffer;
    int position;
};

std::string currentAdminPassword;
std::string currentDomain;

std::vector<std::string> texts;
int numOfUsers = 0;
User users[32];
SOCKET sockets[32];
bool finished[32];
bool hasDomain = false;

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

DWORD WINAPI RegisterServerDomain(LPVOID param)
{
    BufferRequestInitialiseServer* WantedServer = (BufferRequestInitialiseServer*)param;

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
        hasDomain = false;
    }
    else
    {
        char sendBuffer[1];
        sendBuffer[0] = 'I';
        BufferReady BR;
        send(domainSocket, sendBuffer, 1, 0);
        recv(domainSocket, (char*)&BR, sizeof(BR), 0);
        if(BR.isReady)
        {
            BufferResponseInitialiseServer BRIS2;
            send(domainSocket, (char*)WantedServer, sizeof(BufferRequestInitialiseServer), 0);
            recv(domainSocket, (char*)&BRIS2, sizeof(BufferResponseInitialiseServer), 0);
            if(BRIS2.response == 0)
            {
	            
            }

        }

    }

    delete WantedServer;
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
    allServerText.emplace_back("Listening for clients on port: " + currentPort);
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
    char charExternalIp[bufferSize] = "";

    bool wantsToExit = false;
    bool isServerInitialised = false;
    bool isPortOrIPValid = true;

    int previousSize = 0;

    static bool isDiscoverable = false;
    bool isPasswordEmpty = false;
    bool isExternalIpEmpty = false;
    bool isReady = false;

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
                currentIp = IP;
                currentPort = port;
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

        ImGui::SeparatorText("Domain");
        if(isServerInitialised)
        {
            ImGui::InputText("Name", charDomainName, IM_ARRAYSIZE(charDomainName), ImGuiInputTextFlags_EnterReturnsTrue);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Name that people will use to access your chatroom.");
            }
            ImGui::InputText("Password", charPassword, IM_ARRAYSIZE(charPassword), ImGuiInputTextFlags_EnterReturnsTrue);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Optional, allows protection against random people.");
            }
            ImGui::InputText("Admin Password", charAdminPassword, IM_ARRAYSIZE(charAdminPassword), ImGuiInputTextFlags_EnterReturnsTrue);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Used to verify user, in the case of modify chatrooms.");
            }
            ImGui::InputText("External Ip", charExternalIp, IM_ARRAYSIZE(charExternalIp), ImGuiInputTextFlags_EnterReturnsTrue);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Required for others to access your chatroom. Refer to help.txt on more guidance on how to obtain your external ip.");
            }
            ImGui::Checkbox("Is discoverable", &isDiscoverable);
            if (ImGui::IsItemHovered())
            {
                ImGui::SetTooltip("Allows people to know that your server exists, by typing the name of your chatroom name.");
            }
                

            if (ImGui::Button("List server domain"))
            {
                std::string passwordS = charPassword;
                std::string SExternalIp = charExternalIp;
                if (SExternalIp.empty())
                {
                    isReady = false;
                    isExternalIpEmpty = true;
                }
                else if (passwordS.empty())
                {

                    isReady = false;
                    isPasswordEmpty = true;
                }
                else
                {
                    isReady = true;
                }
            }

            if(isExternalIpEmpty)
            {
                ImGui::OpenPopup("External Ip");

                if (ImGui::BeginPopupModal("External Ip", NULL, ImGuiWindowFlags_MenuBar))
                {
                    ImGui::Text("Missing information!");
                    ImGui::TextWrapped("You need to input your external ip, refer to help.txt on more guidance on how to obtain your external ip.");
                    if (ImGui::Button("Okay"))
                    {
                        isExternalIpEmpty = false;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::EndPopup();
                }
            }

            if(isPasswordEmpty)
            {
                ImGui::OpenPopup("Password");

                if (ImGui::BeginPopupModal("Password", NULL, ImGuiWindowFlags_MenuBar))
                {
                    ImGui::Text("WARNING!");
                    ImGui::TextWrapped("You are creating a chatroom without a password. There is a chance of unwanted people from joining your chatroom.");
                    ImGui::Text("Do you wish to continue?");
                    if (ImGui::Button("Yes"))
                    {
                        isPasswordEmpty = false;
                        ImGui::CloseCurrentPopup();
                        isReady = true;
                    }
                    if (ImGui::Button("No"))
                    {
                        isPasswordEmpty = false;
                        isReady = false;
                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::EndPopup();
                }
            }

            if (isReady && !hasDomain)
            {
                std::string nameS = charDomainName;
                std::string passwordS = charPassword;
                std::string adminPasswordS = charAdminPassword;

                isPasswordEmpty = false;
                hasDomain = true;
                isReady = false;

                if (nameS.empty())
                {
                    currentError = "Domain name field is empty";
                }
                else if (adminPasswordS.empty())
                {
                    currentError = "Domain admin password field is empty";
                }
                else if (nameS.size() > 50)
                {
                    currentError = "Domain name cannot exceed 50 characters.";
                }
                else if (passwordS.size() > 50)
                {
                    currentError = "Domain password cannot exceed 50 characters.";
                }
                else if (adminPasswordS.size() > 50)
                {
                    currentError = "Domain admin password cannot exceed 50 characters.";
                }
                else
                {
                    BufferRequestInitialiseServer* DH = new BufferRequestInitialiseServer{ charDomainName, charPassword, charAdminPassword, charExternalIp, std::stoi(currentPort), isDiscoverable};
                    DWORD threadid;
                    HANDLE hdl;
                    hdl = CreateThread(NULL, 0, RegisterServerDomain, (LPVOID)DH, 0, &threadid);
                }
                
            }
            
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

    if(hasDomain)
    {
        SOCKET domainSocket = INVALID_SOCKET;

        domainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (domainSocket == INVALID_SOCKET)
        {
            currentError = "Error creating domain socket.";
            WSACleanup();
            return 0;
        }

        PCWSTR ip = L"127.0.0.1";
        sockaddr_in service;
        service.sin_family = AF_INET;
        service.sin_port = htons(4096);
        InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

        if (connect(domainSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
        {
            allServerText.emplace_back("Connecting to the domain server was unsuccessful.");
            hasDomain = false;
        }
        else
        {
            int lengthOfPassword = currentAdminPassword.size();
            int lengthOfDomain = currentDomain.size();
            std::string buffer = "D" + std::to_string(lengthOfPassword) + "B" + std::to_string(lengthOfDomain) + "B" + currentAdminPassword + currentDomain;
            std::string resultBuffer;
            send(domainSocket, buffer.c_str(), bufferSize, 0);

        }
    }



    WSACleanup();
    gui::DestroyImGui();
    gui::DestroyDevice();
    gui::DestroyHWindow();
    return EXIT_SUCCESS;
}
