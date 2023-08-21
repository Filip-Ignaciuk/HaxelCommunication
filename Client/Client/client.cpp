#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>

#include "user.hpp"
#include "gui.h"	

const int bufferSize = 200;
SOCKET clientSocket;
bool isTextInputThreadFinished = true;
User userClient("PLACEHOLDER", "999");
HANDLE currentTextInputThread;
std::vector<std::string> allTextsInChatRoom = {"In chat room.","Type /h for help" };


int __stdcall wWinMain(HINSTANCE _instace, HINSTANCE _previousInstance, PWSTR _arguments, int commandShow)
{
	gui::CreateHWindow("Cold", "Coldness");
	gui::CreateDevice();
	gui::CreateImGui();

	while(gui::exit)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}


	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();
	return EXIT_SUCCESS;
}

void EditUser()
{
	bool isEditing = true;
	while(isEditing)
	{
		std::string editInput;
		std::cout << "What would you like to edit." << std::endl;
		std::cout << "d - Display name" << std::endl;
		std::getline(std::cin, editInput);
		if (editInput == "d" || editInput == "D")
		{
			std::cout << "Please enter the new display name you would like. " << std::endl;
			std::cout << "If you wish to revert you can always type /q" << std::endl;
			std::cout << "Display name:" << std::endl;
			std::getline(std::cin, editInput);

			if(editInput != "/q" || editInput != "/Q")
			{
				int bytecount = send(clientSocket, editInput.c_str(), bufferSize, 0);
				bytecount = recv(clientSocket, (char*)&userClient, sizeof(User), 0);
				std::cout << "Display name is now: " << userClient.GetDisplayName() << std::endl;
			}

		}
		else if (editInput == "/q" || editInput == "/Q")
		{
			isEditing = false;
		}
		else
		{
			std::cout << "Invalid edit input." << std::endl;
			std::cout << "Please reference the text above and below, to see what is possible to edit." << std::endl;

		}
	}
	
}



DWORD WINAPI SendAndRecieveTextThread(LPVOID param)
{
	std::string* message = (std::string*)param;
	std::string messageWithUser = userClient.GetDisplayName() + ", ID: " + userClient.GetId() + " >> " + *message;
	int messageSize = messageWithUser.size();
	std::string messageFinal = "M" + std::to_string(messageSize) + messageWithUser;
	int bytecount = send(clientSocket, messageFinal.c_str(), bufferSize, 0);
	delete message;

	char buffer[bufferSize];
	bytecount = recv(clientSocket, buffer, bufferSize, 0);
	if (buffer[0] == 'M')
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
		DWORD exitCode = 0;
		isTextInputThreadFinished = false;
		TerminateThread(currentTextInputThread, exitCode);
		system("cls");
		for (std::string text : allTextsInChatRoom)
		{
			std::cout << text << std::endl;
		}
		DWORD threadid;
		isTextInputThreadFinished = true;

		return 0;

	}
}

DWORD WINAPI TextInputThread(LPVOID param)
{
	bool* isReadyToStayInChatRoom = (bool*)param;
	std::string textInput;
	std::cout << "Checking Text" << std::endl;
	std::cin >> textInput;
	std::cout << "Message was " << textInput << std::endl;
	if (textInput[0] == '/')
	{
		if (textInput == "/h" || textInput == "/H")
		{
			std::cout << "Here are all of the commands in chat room" << std::endl;
			std::cout << "/q - Transverse back from current scope, use twice from chatroom or once in settings to leave application." << std::endl;
			std::cout << "Anything that doesnt contain / as the first charater is treated as a text message." << std::endl;
		}
		else if (textInput == "/q" || textInput == "/Q")
		{
			*isReadyToStayInChatRoom = false;
		}
	}
	else
	{
		DWORD threadid;
		HANDLE hdl;
		std::string* textInputPara = new std::string(textInput);
		hdl = CreateThread(NULL, 0, SendAndRecieveTextThread, textInputPara, 0, &threadid);
	}
	isTextInputThreadFinished = true;
	return 0;
}

int main()
{
	


	

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

	std::cout << "IP: " << std::endl;
	std::string ipInput;
	std::getline(std::cin, ipInput);
	// converting ip input to const wchar_t
	std::wstring wideIpInput = std::wstring(ipInput.begin(), ipInput.end());
	PCWSTR ip = wideIpInput.c_str();

	std::cout << "Port: " << std::endl;
	std::string portInput;
	std::getline(std::cin, portInput);

	sockaddr_in service;
	int port = std::stoi(portInput);
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		std::cout << "Binding failed." << std::endl;
		std::cout << "Ip or port number may be invalid, not open or perhaps misspelled" << std::endl;
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}

	std::cout << "Successfully connected to server." << std::endl;
	std::cout << "Display Name:" << std::endl;
	std::string displayName;
	std::getline(std::cin, displayName);
	// Need to optimise 
	std::string bufferString = "I";
	int size = displayName.size();
	bufferString.append(std::to_string(size));
	bufferString.append(displayName);
	const char* buffer = bufferString.c_str();
	send(clientSocket, buffer, bufferSize, 0);
	int bytecount = recv(clientSocket, (char*)&userClient, sizeof(User), 0);
	std::cout << "Name is: " << userClient.GetDisplayName() << " and ID is: " << userClient.GetId() << std::endl;
	


	std::cout << "Would you like to enter the chat room. (Y/N)" << std::endl;
	std::string input;
	std::getline(std::cin, input);
	bool isReady = input == "Y" || input == "y";
	bool final = true;
	bool isReadyToStayInSettings = true;
	bool isReadyToStayInChatRoom = true;
	if (final)
	{
		// Equivalent to settings.
		while (isReadyToStayInSettings)
		{
			if (isReady)
			{
				std::cout << "In chat room." << std::endl;
				std::cout << "Type /h for help" << std::endl;
				// Equivalent to chat room.
				while (isReadyToStayInChatRoom)
				{
					if(isTextInputThreadFinished)
					{
						std::cout << "Creating Thread" << std::endl;
						DWORD threadid;
						isTextInputThreadFinished = false;
						currentTextInputThread = CreateThread(NULL, 0, TextInputThread, &isReadyToStayInChatRoom, 0, &threadid);
					}
					
				}
				std::cout << "Leaving chat room." << std::endl;
				isReady = false;
			}
			std::cout << "In settings." << std::endl;
			std::cout << "Type /h for help" << std::endl;
			std::getline(std::cin, input);
			// So that we dont have to compare every single one if its not a command.
			if(input[0] == '/')
			{
				if (input == "/h" || input == "/H")
				{
					std::cout << "Here are all of the commands in settings" << std::endl;
					std::cout << "/e - Enter chat room." << std::endl;
					std::cout << "/q - Transverse back from current scope, use twice from chatroom or once in settings to leave application." << std::endl;
					std::cout << "/u - Update your user profile" << std::endl;
				}
				else if (input == "/e" || input == "/E")
				{
					std::cout << "Entering chat room." << std::endl;
					isReady = true;
				}
				else if (input == "/q" || input == "/Q")
				{
					std::cout << "Leaving Haxel communication." << std::endl;
					isReadyToStayInSettings = false;
				}
				else if (input == "/u" || input == "/U")
				{
					std::cout << "Editing user profile." << std::endl;
					EditUser();
				}
			}
		}

		
	}
	
	closesocket(clientSocket);
	WSACleanup();

}