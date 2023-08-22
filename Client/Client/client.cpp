#include <filesystem>

#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <iso646.h>
#include <vector>
#include <thread>

#include "user.hpp"
#include "gui.h"	
#include "ImGui/imgui.h"

const int bufferSize = 200;
SOCKET clientSocket;
User clientUser("PLACEHOLDER", "999");

std::vector<std::string> allTextsInChatRoom = {"In chat room.","Type /h for help" };

int CreateSocket()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(version, &wsaData))
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
}





DWORD WINAPI SendTextThread(LPVOID param)
{
	std::string* message = (std::string*)param;
	std::string messageWithUser = clientUser.GetDisplayName() + ", ID: " + clientUser.GetId() + " >> " + *message;
	int messageSize = messageWithUser.size();
	std::string messageFinal = "M" + std::to_string(messageSize) + messageWithUser;
	int bytecount = send(clientSocket, messageFinal.c_str(), bufferSize, 0);
	delete message;
	return 0;
}


const ImVec4 notConnectedColour = ImVec4(0.1, 0.8, 0.9, 1);
const ImVec4 tryingToConnectColour = ImVec4(0.90, 0.8, 0.12, 1);
const ImVec4 connectedColour = ImVec4(0.16, 0.9, 0.1, 1);
const ImVec4 failedToConnectColour = ImVec4(1, 0, 0, 1);

std::string currentConnectionStatus = "Not connected";
std::string currentStatusChangingUser = "Not connected";
ImVec4 currentColourConnection = notConnectedColour;
ImVec4 currentColourChangingUser = notConnectedColour;

bool isConnected = false;

struct ConnectHolder
{
	char* charIp;
	char* charPort;
};

struct DisplayNameHolder
{
	char* charDisplayName;
};

bool isConnecting = false;
bool isInitialised = false;
bool isChangingDisplayName = false;
bool isRecieving = false;
bool expectUser = false;

DWORD WINAPI TryToConnectThread(LPVOID param)
{
	ConnectHolder* CH = (ConnectHolder*)param;
	// Converting ip input to const wchar_t
	std::string stringIp = CH->charIp;
	std::wstring wideIpInput = std::wstring(stringIp.begin(), stringIp.end());
	PCWSTR ip = wideIpInput.c_str();

	int port = std::stoi(CH->charPort);

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		isConnecting = false;
		currentConnectionStatus = "Failed to connect";
		currentColourConnection = failedToConnectColour;
		WSACleanup();
		delete CH;
	}
	else
	{
		isConnecting = false;
		isConnected = true;
		currentConnectionStatus = "Connected";
		currentColourConnection = connectedColour;
		return 0;
		delete CH;
	}
	
}

DWORD WINAPI ConnectingThread(LPVOID param)
{
	int i = 0;
	std::string connectingArray[3] = { "Connecting.", "Connecting..", "Connecting..." };
	currentConnectionStatus = connectingArray[0];
	currentColourConnection = tryingToConnectColour;
	while (isConnecting)
	{
		currentConnectionStatus = connectingArray[i];
		i++;
		if (i == 3)
		{
			i = 0;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	return 0;
}

DWORD WINAPI InitialiseUserThread(LPVOID param)
{
	DisplayNameHolder* DNH = (DisplayNameHolder*)param;
	std::string displayName = DNH->charDisplayName;
	std::string bufferString = "I";
	int size = displayName.size();
	bufferString.append(std::to_string(size));
	bufferString.append(displayName);
	const char* buffer = bufferString.c_str();
	send(clientSocket, buffer, bufferSize, 0);
	expectUser = true;
	isChangingDisplayName = true;
	return 0;
	
}

DWORD WINAPI ChangeDisplayNameThread(LPVOID param)
{
	DisplayNameHolder* DNH = (DisplayNameHolder*)param;
	std::string displayName = DNH->charDisplayName;
	std::string bufferString = "UD";
	int size = displayName.size();
	bufferString.append(std::to_string(size));
	bufferString.append(displayName);
	const char* buffer = bufferString.c_str();
	send(clientSocket, buffer, bufferSize, 0);
	expectUser = true;
	isChangingDisplayName = true;
	return 0;
}

DWORD WINAPI ChangingThread(LPVOID param)
{
	int i = 0;
	std::string connectingArray[3] = { "Changing.", "Changing..", "Changing..." };
	currentStatusChangingUser = connectingArray[0];
	currentColourChangingUser = tryingToConnectColour;
	while (isChangingDisplayName)
	{
		currentStatusChangingUser = connectingArray[i];
		i++;
		if (i == 3)
		{
			i = 0;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}

DWORD WINAPI RecieveThread(LPVOID param)
{
	std::string buffer;

	int bytecount = recv(clientSocket, (char*)&buffer, bufferSize, 0);

	if(expectUser)
	{
		User* user = (User*)&buffer;
		clientUser = *user;
		isChangingDisplayName = false;
		currentStatusChangingUser = "Successfully updated user information.";
		currentColourChangingUser = connectedColour;
		isChangingDisplayName = false;
	}
	if (bytecount == 0)
	{
		
	}
	else if (buffer[0] == 'M')
	{
		std::string message;
		std::string LengthS = "";
		int j = 1;
		while (isdigit(buffer[j]))
		{
			LengthS.push_back(buffer[j]);
			j++;
		}
		int length = std::stoi(LengthS) + j;
		for (int i = j; i < length; i++)
		{
			message.push_back(buffer[i]);
		}
		allTextsInChatRoom.emplace_back(message);
	}
	else
	{

	}

	isRecieving = false;
	return 0;
}


int __stdcall wWinMain(HINSTANCE _instace, HINSTANCE _previousInstance, PWSTR _arguments, int commandShow)
{
	gui::CreateHWindow("Haxel Communication", "haxelClass");
	gui::CreateDevice();
	gui::CreateImGui();

	char charIp[bufferSize] = "";
	char charPort[bufferSize] = "";
	char charDisplayName[bufferSize] = "";
	static char messageText[bufferSize] = "";

	CreateSocket();


	while (gui::exit)
	{
		gui::BeginRender();
		gui::Render();

		

		ImGui::SeparatorText("Connection");
		// Ip
		ImGui::InputText("IP", charIp, IM_ARRAYSIZE(charIp));
		

		// Port
		ImGui::InputText("Port", charPort, IM_ARRAYSIZE(charPort));
		

		// Status
		
		ImGui::TextColored(currentColourConnection, currentConnectionStatus.c_str());

		if (ImGui::Button("Connect"))
		{
			// Just in case so that it doesn't spam threads.
			if (!isConnecting)
			{
				isConnecting = true;
				DWORD threadid;
				HANDLE hdl;
				ConnectHolder* CH = new ConnectHolder{ charIp, charPort };
				hdl = CreateThread(NULL, 0, TryToConnectThread, CH, 0, &threadid);
				hdl = CreateThread(NULL, 0, ConnectingThread, 0, 0, &threadid);
			}

		}

		if(isConnected)
		{
			if (!isRecieving)
			{
				isRecieving = true;
				DWORD threadid;
				HANDLE hdl;
				hdl = CreateThread(NULL, 0, RecieveThread, 0, 0, &threadid);
			}

			ImGui::SeparatorText("User");
			ImGui::InputText("Display Name", charDisplayName, IM_ARRAYSIZE(charDisplayName));
			ImGui::TextColored(currentColourChangingUser, currentStatusChangingUser.c_str());
			if(ImGui::Button("Update user"))
			{
				if(!isChangingDisplayName)
				{
					isChangingDisplayName = true;
					DWORD threadid;
					HANDLE hdl;
					DisplayNameHolder* DNH = new DisplayNameHolder{ charDisplayName };
					if(isInitialised)
					{
						hdl = CreateThread(NULL, 0, ChangeDisplayNameThread, DNH, 0, &threadid);
					}
					else
					{
						hdl = CreateThread(NULL, 0, InitialiseUserThread, DNH, 0, &threadid);
						isInitialised = true;
					}
					hdl = CreateThread(NULL, 0, ChangingThread, 0, 0, &threadid);
					
				}
			}

			ImGui::SeparatorText("Chat Room");
			ImGui::BeginChild("Scrolling");
			for (int i = 0; i < allTextsInChatRoom.size(); i++)
			{
				ImGui::TextColored(ImVec4(1, 1, 0, 1), allTextsInChatRoom[i].c_str());
			}
			ImGui::EndChild();
			ImGui::InputText("messageText", messageText, IM_ARRAYSIZE(messageText));
			
		}

		




		ImGui::End();
		gui::EndRender();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	closesocket(clientSocket);
	WSACleanup();
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();
	return EXIT_SUCCESS;
}
