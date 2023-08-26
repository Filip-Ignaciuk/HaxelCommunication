#include <filesystem>
#include <fstream>
#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>

#include "user.hpp"
#include "gui.h"	
#include "ImGui/imgui.h"

// Converting all the \\ slashes into one singular forward slash.
std::string NormaliseDir(std::string& _str)
{
	for (unsigned int i = 0; i < _str.size(); i++)
	{
		if (_str[i] == '\\')
		{
			_str[i] = '/';
		}
	}
	return _str;
}

const int bufferSize = 200;
SOCKET clientSocket;
User clientUser("PLACEHOLDER", "999");
std::string currentDir = std::filesystem::current_path().string();
std::string currentDirNormalised = NormaliseDir(currentDir);

void InitFolders()
{
	if (std::filesystem::create_directory(currentDirNormalised + "/Languages") == 0)
	{
		std::cin.get();
	}

	if (std::filesystem::create_directory(currentDirNormalised + "/Users") == 0)
	{
		std::cin.get();
	}
}




int CreateSocket()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(version, &wsaData))
	{	
		std::cout << WSAGetLastError() << std::endl;
		return 0;
	}


	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return 0;
	}

}

DWORD WINAPI SendTextThread(LPVOID param)
{
	std::string* messageptr = (std::string*)param;
	std::string messageWithUser = clientUser.GetDisplayName() + ", ID: " + clientUser.GetId() + " >> " + *messageptr;
	int messageSize = messageWithUser.size();
	std::string messageFinal = "M" + std::to_string(messageSize) + messageWithUser;
	int bytecount = send(clientSocket, messageFinal.c_str(), bufferSize, 0);
	delete messageptr;
	return 0;
}

const unsigned char numOfSentences = 25;

std::string allTextsInApplication[numOfSentences];
const char* charAllTextsInApplication[numOfSentences];

std::vector<std::string> allTextsInChatRoom;


HANDLE recieveThread;

const ImVec4 notConnectedColour = ImVec4(0.1, 0.8, 0.9, 1);
const ImVec4 tryingToConnectColour = ImVec4(0.90, 0.8, 0.12, 1);
const ImVec4 connectedColour = ImVec4(0.16, 0.9, 0.1, 1);
const ImVec4 failedToConnectColour = ImVec4(1, 0, 0, 1);

std::string currentConnectionStatus = "";
std::string currentStatusChangingUser = "";
ImVec4 currentColourConnection = notConnectedColour;
ImVec4 currentColourChangingUser = notConnectedColour;

bool isConnected = false;

std::string wantedDisplayName;

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
bool isApplicationInitialised = false;
bool isChangingDisplayName = false;
bool isRecieving = false;
bool isTimedOut = false;

int language;
const std::string languages[2] = { "/en-gb.txt", "/pl.txt" };

HANDLE timedOutHandles[3];
int numOfTimedOutThreads = 0;

DWORD WINAPI TimedOutThread(LPVOID param)
{
	Sleep(25000);
	isTimedOut = true;
	currentConnectionStatus = allTextsInApplication[26];
	currentColourConnection = failedToConnectColour;
}
void StartTimedOutThread()
{
	if(numOfTimedOutThreads != 3)
	{
		DWORD threadid;
		timedOutHandles[numOfTimedOutThreads] = CreateThread(NULL, 0, TimedOutThread, 0, 0, &threadid);
		numOfTimedOutThreads++;
	}
}
void StopTimedOutThread()
{
	// ToDO
	//TerminateThread();
}

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
		currentConnectionStatus = allTextsInApplication[6];
		currentColourConnection = failedToConnectColour;
		delete CH;
	}
	else
	{
		isConnecting = false;
		isConnected = true;
		currentConnectionStatus = allTextsInApplication[4];
		currentColourConnection = connectedColour;
		return 0;
		delete CH;
	}
	
}

DWORD WINAPI ConnectingThread(LPVOID param)
{
	int i = 0;
	std::string connectingArray[3] = { allTextsInApplication[7], allTextsInApplication[8], allTextsInApplication[9] };
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
	wantedDisplayName = displayName;
	std::string bufferString = "I";
	int size = displayName.size();
	bufferString.append(std::to_string(size));
	bufferString.append(displayName);
	const char* buffer = bufferString.c_str();
	send(clientSocket, buffer, bufferSize, 0);
	
	return 0;
	
}

DWORD WINAPI ChangeDisplayNameThread(LPVOID param)
{
	DisplayNameHolder* DNH = (DisplayNameHolder*)param;
	std::string displayName = DNH->charDisplayName;
	wantedDisplayName = displayName;
	std::string bufferString = "UD";
	int size = displayName.size();
	bufferString.append(std::to_string(size));
	bufferString.append(displayName);
	const char* buffer = bufferString.c_str();
	send(clientSocket, buffer, bufferSize, 0);
	
	return 0;
}

DWORD WINAPI ChangingThread(LPVOID param)
{
	int i = 0;
	std::string connectingArray[3] = { allTextsInApplication[12],allTextsInApplication[13], allTextsInApplication[14] };
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
	char buffer[bufferSize];
	int bytecount = recv(clientSocket, buffer, bufferSize, 0);

	if (bytecount == 0)
	{
		isRecieving = false;
		return 0;
	}

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
	}
	else if (buffer[0] == 'I')
	{
		isChangingDisplayName = false;
		currentStatusChangingUser = allTextsInApplication[15];
		currentColourChangingUser = connectedColour;
		std::string id;
		if(buffer[1] == '1')
		{
			id = " ";
			id[0] = buffer[2];
		}
		else
		{
			id = "  ";
			id[0] = buffer[2];
			id[1] = buffer[3];
		}

		const User user(wantedDisplayName, id);
		clientUser = user;
		isInitialised = true;
		
	}
	else if (buffer[0] == 'U')
	{
		isChangingDisplayName = false;
		currentStatusChangingUser = allTextsInApplication[21];
		currentColourChangingUser = connectedColour;
		clientUser.SetDisplayName(wantedDisplayName);
		
	}
	else if (buffer[0] == 'E')
	{
		if(buffer[0] == 'M')
		{
			currentConnectionStatus = allTextsInApplication[25];
			currentColourChangingUser = notConnectedColour;
		}
	}

	isRecieving = false;
	return 0;
}

void AddHelpText()
{
	const std::string helpers[3] = {};
}

void InitLanguageFiles()
{
	const std::string langWord = "/Languages";
	std::ofstream fileengb(currentDirNormalised + langWord + languages[0]);
	fileengb << "IP" << std::endl;
	fileengb << "Port" << std::endl;
	fileengb << "Connection" << std::endl;
	fileengb << "Not connected" << std::endl;
	fileengb << "Connected" << std::endl;
	fileengb << "Connect " << std::endl;
	fileengb << "Failed to connect" << std::endl;
	fileengb << "Connecting." << std::endl;
	fileengb << "Connecting.." << std::endl;
	fileengb << "Connecting..." << std::endl;
	fileengb << "User" << std::endl;
	fileengb << "Display Name" << std::endl;
	fileengb << "Changing." << std::endl;
	fileengb << "Changing.." << std::endl;
	fileengb << "Changing..." << std::endl;
	fileengb << "Successfully initialised user" << std::endl;
	fileengb << "Update user" << std::endl;
	fileengb << "Chat room" << std::endl;
	fileengb << "In chat room" << std::endl;
	fileengb << "Type /h for help" << std::endl;
	fileengb << "Message" << std::endl;
	fileengb << "Successfully changed user data" << std::endl;
	fileengb << "Languages" << std::endl;
	fileengb << "Display Name is too large" << std::endl;
	fileengb << "Invalid port or IP" << std::endl;
	fileengb << "Max clients within chatroom! Please wait for someone to leave or join another one." << std::endl;
	fileengb << "Request timed out. Please try to reconnect to the server." << std::endl;
	fileengb.close();

	std::ofstream filepl(currentDirNormalised + langWord + languages[1]);
	filepl.close();
}

void InitialiseApplication()
{
	int i = 0;
	std::ifstream languageFile(currentDirNormalised + "/Languages" + languages[language]);
	if(languageFile.is_open())
	{
		std::string line;
		while(std::getline(languageFile, line))
		{
			allTextsInApplication[i] = line;
			charAllTextsInApplication[i] = allTextsInApplication[i].c_str();
			i++;
		}
	}
	else
	{
		
	}
	currentConnectionStatus = allTextsInApplication[3];
	currentStatusChangingUser = allTextsInApplication[3];
	allTextsInChatRoom.emplace_back(allTextsInApplication[18]);
	allTextsInChatRoom.emplace_back(allTextsInApplication[19]);
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

	bool isTooLarge = false;
	bool isPortOrIPValid = true;
	

	CreateSocket();

	std::ifstream file;
	file.open(currentDirNormalised + "/initialised.txt");
	if (!file.is_open())
	{
		InitFolders();
		file.close();
		std::ofstream file(currentDirNormalised + "/initialised.txt");
		file << "1" << std::endl;
		file << "0" << std::endl;
		language = 0;
		file.close();
		InitLanguageFiles();
	}
	isApplicationInitialised = true;

	InitialiseApplication();

	
	

	while (gui::exit)
	{
		gui::BeginRender();
		gui::Render();

		//if (ImGui::BeginMainMenuBar())
		//{
		//	if (ImGui::BeginMenu("Open Recent"))
		//	{
		//		ImGui::MenuItem("fish_hat.c");
		//		ImGui::MenuItem("fish_hat.inl");
		//		ImGui::MenuItem("fish_hat.h");
		//
		//		ImGui::EndMenu();
		//	}
		//	ImGui::EndMainMenuBar();
		//}

		
		

		ImGui::SeparatorText(charAllTextsInApplication[3]);

		// Ip
		ImGui::InputText(charAllTextsInApplication[0], charIp, IM_ARRAYSIZE(charIp));

		// Port
		ImGui::InputText(charAllTextsInApplication[1], charPort, IM_ARRAYSIZE(charPort));

		// Status
		ImGui::TextColored(currentColourConnection, currentConnectionStatus.c_str());

		if (!isPortOrIPValid)
		{
			ImGui::Text(charAllTextsInApplication[24]);
		}

		if (ImGui::Button(charAllTextsInApplication[5]))
		{
			std::string IP = charIp;
			for (unsigned char i = 0; i < IP.size(); i++)
			{
				if(isdigit(IP[i]) || IP[i] == '.')
				{
					
				}
				else
				{
					isPortOrIPValid = false;
					break;
				}
			}
			std::string port = charPort;
			for(unsigned char j = 0; j < port.size(); j++)
			{
				if(isdigit(port[j]))
				{
					
				}
				else
				{
					isPortOrIPValid = false;
					break;
				}
			}

			// Just in case so that it doesn't spam threads.
			if (!isConnecting && isPortOrIPValid)
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
				recieveThread = CreateThread(NULL, 0, RecieveThread, 0, 0, &threadid);
			}

			ImGui::SeparatorText(charAllTextsInApplication[10]);
			if(isInitialised)
			{
				ImGui::Text(clientUser.GetDisplayName().c_str());
			}
			ImGui::InputText(charAllTextsInApplication[11], charDisplayName, IM_ARRAYSIZE(charDisplayName));
			ImGui::TextColored(currentColourChangingUser, currentStatusChangingUser.c_str());
			if(isTooLarge)
			{
				ImGui::Text(charAllTextsInApplication[23]);
			}
			if(ImGui::Button(charAllTextsInApplication[16]))
			{
				std::string name = charDisplayName;
				if(name.size() > 50)
				{
					isTooLarge = true;
				}
				else
				{
					isTooLarge = false;
				}
				
				if(!isChangingDisplayName && !isTooLarge)
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
					}
					hdl = CreateThread(NULL, 0, ChangingThread, 0, 0, &threadid);
					
				}
			}
			if(isInitialised)
			{
				ImGui::SeparatorText(charAllTextsInApplication[17]);
				ImGui::BeginChild("Scrolling");
				for (int i = 0; i < allTextsInChatRoom.size(); i++)
				{
					ImGui::TextColored(ImVec4(1, 1, 1, 1), allTextsInChatRoom[i].c_str());
				}
				ImGui::EndChild();
				if(ImGui::InputText(charAllTextsInApplication[20], messageText, IM_ARRAYSIZE(messageText), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					if(messageText[0] == '/')
					{
						if(messageText[1] == 'h')
						{
							AddHelpText();
						}
					}
					else
					{
						DWORD threadid;
						HANDLE hdl;
						std::string* messagePtr = new std::string(messageText);
						hdl = CreateThread(NULL, 0, SendTextThread, messagePtr, 0, &threadid);
					}
					
				}
				
				
			}
		}

		




		ImGui::End();
		gui::EndRender();
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	const char* quitBuffer = "Q";
	send(clientSocket, quitBuffer, bufferSize, 0);
	closesocket(clientSocket);
	WSACleanup();
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();
	return EXIT_SUCCESS;
}
