#include <filesystem>
#include <fstream>
#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <thread>
#include <ctime>

#include "BufferStandard.hpp"
#include "Languages.hpp"
#include "config.hpp"
#include "user.hpp"
#include "gui.h"	
#include "ImGui/imgui.h"

SOCKET clientSocket;
User clientUser("PLACEHOLDER", "999");

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

	return 0;

}
std::vector<std::string> allTextsInChatRoom;

User users[32];

const ImVec4 notConnectedColour = ImVec4(0.1f, 0.8f, 0.9f, 1);
const ImVec4 tryingToConnectColour = ImVec4(0.90f, 0.8f, 0.12f, 1);
const ImVec4 connectedColour = ImVec4(0.16f, 0.9f, 0.1f, 1);
const ImVec4 failedToConnectColour = ImVec4(1, 0, 0, 1);

std::string currentConnectionStatus = "";
std::string currentStatusChangingUser = "";
ImVec4 currentColourConnection = notConnectedColour;
ImVec4 currentColourChangingUser = notConnectedColour;

bool isConnected = false;

std::string wantedDisplayName;

struct ConnectHolder
{
	std::string ip;
	std::string port;
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


DWORD WINAPI SendTextThread(LPVOID param)
{
	std::string* messageptr = (std::string*)param;
	std::string messageWithUser = clientUser.GetDisplayName() + " >> " + *messageptr;
	int messageSize = messageWithUser.size();
	std::string messageFinal = "M" + std::to_string(messageSize) + messageWithUser;
	int bytecount = send(clientSocket, messageFinal.c_str(), bufferSize, 0);
	delete messageptr;
	return 0;
}

DWORD WINAPI TryToConnectThread(LPVOID param)
{
	ConnectHolder* CH = (ConnectHolder*)param;
	// Converting ip input to const wchar_t
	std::string stringIp = CH->ip;
	std::wstring wideIp = std::wstring(stringIp.begin(), stringIp.end());
	PCWSTR ip = wideIp.c_str();
	int port = std::stoi(CH->port);
	delete CH;

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

	int result = connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service));
	if (result)
	{
		isConnecting = false;
		currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[6];
		currentColourConnection = failedToConnectColour;
	}
	else
	{
		isConnecting = false;
		isConnected = true;
		currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[4];
		currentColourConnection = connectedColour;
		
	}
	return 0;
	
}



DWORD WINAPI ConnectingThread(LPVOID param)
{
	int i = 0;
	std::string connectingArray[3] = {LanguageFileInitialiser::allTextsInApplication[7], LanguageFileInitialiser::allTextsInApplication[8], LanguageFileInitialiser::allTextsInApplication[9] };
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
	std::string connectingArray[3] = {LanguageFileInitialiser::allTextsInApplication[12], LanguageFileInitialiser::allTextsInApplication[13], LanguageFileInitialiser::allTextsInApplication[14] };
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

void CleanupSocket()
{
	shutdown(clientSocket, SD_BOTH);
	closesocket(clientSocket);
	WSACleanup();
	CreateSocket();
}

DWORD WINAPI RequestDomainThread(LPVOID param)
{
	ConnectHolder* CH = (ConnectHolder*)param;
	std::string domainName = CH->ip;
	std::string domainPassword = CH->port;
	delete CH;

	PCWSTR ip = config::WDomainIp.c_str();
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(config::IDomainPort);
	InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		isConnecting = false;
		currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[28];
		currentColourConnection = failedToConnectColour;
		closesocket(clientSocket);
		WSACleanup();
		CreateSocket();
		return 0;
	}
	else
	{
		BufferReady BR;
		send(clientSocket, (char*)'R', 1, 0);
		recv(clientSocket, (char*)&BR, sizeof(BufferReady), 0);
		if(BR.isReady)
		{
			BufferRequestIp BRI;
			BRI.requestedDomain = domainName;
			BRI.requestedDomainPassword = domainPassword;
			BufferResponseIp BRI2;
			send(clientSocket, (char*)&BRI, sizeof(BufferRequestIp), 0);
			recv(clientSocket, (char*)&BRI2, sizeof(BufferResponseIp), 0);

			if(BRI.requestedDomain == "2" && BRI2.responsePort == 2)
			{
				isConnecting = false;
				currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[29];
				currentColourConnection = failedToConnectColour;
				CleanupSocket();
				return 0;
			}
			else if (BRI.requestedDomain == "1" && BRI2.responsePort == 1)
			{
				isConnecting = false;
				currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[30];
				currentColourConnection = failedToConnectColour;
				CleanupSocket();
				return 0;
			}
			else
			{
				DWORD threadid;
				HANDLE hdl;
				ConnectHolder* CH2 = new ConnectHolder{ BRI.requestedDomain , std::to_string(BRI2.responsePort) };
				CleanupSocket();
				hdl = CreateThread(NULL, 0, TryToConnectThread, CH2, 0, &threadid);
				return 0;
			}
		}
		else
		{
			isConnecting = false;
			currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[29];
			currentColourConnection = failedToConnectColour;
			CleanupSocket();
			return 0;
		}
	}


}


DWORD WINAPI ReceiveThread(LPVOID param)
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
		std::string defaultMessage;
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
			defaultMessage.push_back(buffer[i]);
		}
		// Add on stuff based upon the person's preference.
		if(config::isTimeFormatOn)
		{
			const time_t timeNow = time(0);
			tm* ltm = new tm();
			localtime_s(ltm, &timeNow);
			message = "(" + std::to_string(ltm->tm_hour) + ":" + std::to_string(ltm->tm_min) + ") " + defaultMessage;
			delete ltm;
		}
		else
		{
			message = defaultMessage;
		}
		
		allTextsInChatRoom.emplace_back(message);
	}
	else if (buffer[0] == 'I')
	{
		isChangingDisplayName = false;
		currentStatusChangingUser = LanguageFileInitialiser::allTextsInApplication[15];
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
		currentStatusChangingUser = LanguageFileInitialiser::allTextsInApplication[21];
		currentColourChangingUser = connectedColour;
		clientUser.SetDisplayName(wantedDisplayName);
		
	}
	else if (buffer[0] == 'T')
	{
		std::string id;
		std::string sizeOfName;
		int startPos;
		if (buffer[1] == '1')
		{
			id = " ";
			id[0] = buffer[2];
			if(isdigit(buffer[3]) && isdigit(buffer[4]))
			{
				sizeOfName = "  ";
				sizeOfName[0] = buffer[3];
				sizeOfName[1] = buffer[4];
				startPos = 5;
			}
			else
			{
				sizeOfName = " ";
				sizeOfName[0] = buffer[3];
				startPos = 4;
			}
		}
		else if (buffer[0] == 'D')
		{
			
		}
		else
		{
			id = "  ";
			id[0] = buffer[2];
			id[1] = buffer[3];

			if (isdigit(buffer[4]) && isdigit(buffer[5]))
			{
				sizeOfName = "  ";
				sizeOfName[0] = buffer[4];
				sizeOfName[1] = buffer[5];
				startPos = 6;
			}
			else
			{
				sizeOfName = " ";
				sizeOfName[0] = buffer[4];
				startPos = 5;
			}
		}


		std::string bufferMessage = buffer;
		std::string displayName = "";
		int sizeOfMessage = bufferMessage.size();
		for (; startPos < sizeOfMessage; startPos++)
		{
			displayName.push_back(buffer[startPos]);
		}
		User user(displayName, id);
		users[std::stoi(id)] = user;
		
	}
	else if (buffer[0] == 'E')
	{
		if(buffer[1] == 'M')
		{
			currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[25];
			currentColourChangingUser = notConnectedColour;
		}
	}
	else if (buffer[0] == 'Q')
	{
		if(buffer[1] == 'S')
		{
			currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[27];
			currentColourConnection = tryingToConnectColour;
			isConnected = false;
		}
	}

	isRecieving = false;
	return 0;
}

void AddHelpText()
{
	const std::string helpers[3] = {};
}

char charIp[bufferSize] = "";
char charPort[bufferSize] = "";
char charDisplayName[bufferSize] = "";


bool isDisplayNameTooLarge = false;
bool isPortOrIPValid = true;
bool isMessageTooLong = false;
bool isMessageTooShort = false;

bool hasDomainNameFailed = false;


int previousSize = 0;
int sizeOfMessage = 0;

bool languageChanged = false;



void Chatroom()
{
	char messageText[bufferSize] = "";

	ImGui::SeparatorText("Server");
	if (!isConnected)
	{
		// Ip
		ImGui::InputText(LanguageFileInitialiser::charAllTextsInApplication[0], charIp, IM_ARRAYSIZE(charIp));

		// Port
		ImGui::InputText(LanguageFileInitialiser::charAllTextsInApplication[1], charPort, IM_ARRAYSIZE(charPort));

		// Status
		ImGui::TextColored(currentColourConnection, currentConnectionStatus.c_str());

		if (!isPortOrIPValid && hasDomainNameFailed)
		{
			ImGui::Text(LanguageFileInitialiser::charAllTextsInApplication[24]);
		}

		if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[5]))
		{
			isPortOrIPValid = true;
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
					break;
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
			if (dotCount != 3)
			{
				isPortOrIPValid = false;
			}

			if (!isPortOrIPValid)
			{
				isConnecting = true;
				DWORD threadid;
				HANDLE hdl;
				ConnectHolder* CH = new ConnectHolder{ charIp, charPort };
				hdl = CreateThread(NULL, 0, RequestDomainThread, CH, 0, &threadid);
				hdl = CreateThread(NULL, 0, ConnectingThread, 0, 0, &threadid);
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
	}
	if (isConnected)
	{
		if (ImGui::Button("All users"))
		{
			ImGui::OpenPopup("All users");
		}


		if (ImGui::BeginPopupModal("All users", NULL, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginTable("Header", 2))
			{

				for (int row = 0; row < 1; row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						if (column)
						{
							ImGui::Text("Display Name");
						}
						else
						{
							ImGui::Text("ID");
						}

					}
				}
				ImGui::EndTable();
			}
			if (ImGui::BeginTable("Current users in chatroom:", 2))
			{


				for (int row = 0; row < 32; row++)
				{
					ImGui::TableNextRow();
					for (int column = 0; column < 2; column++)
					{
						ImGui::TableSetColumnIndex(column);
						if (column)
						{
							const std::string userDisplayName = users[row].GetDisplayName();
							if (!userDisplayName.empty())
							{
								ImGui::Text(userDisplayName.c_str());
							}

						}
						else
						{
							const std::string userID = users[row].GetId();
							if (!userID.empty())
							{
								ImGui::Text(userID.c_str());
							}
						}
					}
				}
				ImGui::EndTable();
			}
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
		// Status

		ImGui::TextColored(currentColourConnection, currentConnectionStatus.c_str());
		if (ImGui::Button("Exit Chatroom."))
		{
			const char* quitBuffer = "Q";
			send(clientSocket, quitBuffer, bufferSize, 0);
			isConnected = false;
			closesocket(clientSocket);
			WSACleanup();
		}


		if (!isRecieving)
		{
			isRecieving = true;
			DWORD threadid;
			HANDLE hdl = CreateThread(NULL, 0, ReceiveThread, 0, 0, &threadid);
		}

		ImGui::SeparatorText(LanguageFileInitialiser::charAllTextsInApplication[10]);
		if (isInitialised)
		{
			ImGui::Text(clientUser.GetDisplayName().c_str());
		}

		
		if (isInitialised)
		{
			ImGui::SeparatorText(LanguageFileInitialiser::charAllTextsInApplication[17]);
			ImGui::BeginChild("Scrolling");
			const int currentSize = allTextsInChatRoom.size();

			if (previousSize == currentSize)
			{
				for (int i = 0; i < currentSize; i++)
				{
					ImGui::TextWrapped(allTextsInChatRoom[i].c_str());
				}
			}
			else
			{
				for (int i = 0; i < currentSize; i++)
				{
					// Wont work when a new message comes.
					ImGui::TextWrapped(allTextsInChatRoom[i].c_str());
					previousSize = currentSize;
					ImGui::SetScrollHereY(1.0f);
				}
			}




			ImGui::EndChild();
			if (ImGui::InputText(LanguageFileInitialiser::charAllTextsInApplication[20], messageText, IM_ARRAYSIZE(messageText), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				std::string stringMessage = messageText;
				sizeOfMessage = stringMessage.size();
				isMessageTooShort = false;
				isMessageTooLong = false;
				ImGui::SetKeyboardFocusHere(-1);
				if (messageText[0] == '/')
				{
					if (messageText[1] == 'h')
					{
						AddHelpText();
					}
				}

				if (!sizeOfMessage)
				{
					isMessageTooShort = true;
				}
				else if (sizeOfMessage > 120)
				{
					isMessageTooLong = true;
				}
				else
				{
					DWORD threadid;
					HANDLE hdl;
					std::string* messagePtr = new std::string(messageText);
					hdl = CreateThread(NULL, 0, SendTextThread, messagePtr, 0, &threadid);
				}

			}

			if (isMessageTooLong)
			{
				ImGui::Text("Message too large. Consider splitting up your messages.");
			}
			else if (isMessageTooShort)
			{
				ImGui::Text("Message too small");
			}


		}
	}
}

void User()
{

	// User Information
	ImGui::SeparatorText(LanguageFileInitialiser::charAllTextsInApplication[31]);
	ImGui::InputText(LanguageFileInitialiser::charAllTextsInApplication[11], charDisplayName, IM_ARRAYSIZE(charDisplayName));
	// User Customisation
	ImGui::SeparatorText(LanguageFileInitialiser::charAllTextsInApplication[32]);
	static float userColour[3] = { 1.0f, 1.0f, 1.0f };
	ImGui::ColorEdit3("color 1", userColour);
	// User Finialisation
	ImGui::TextColored(currentColourChangingUser, currentStatusChangingUser.c_str());
	if (ImGui::Button(LanguageFileInitialiser::charAllTextsInApplication[16]))
	{
		std::string name = charDisplayName;
		if (name.size() > 50)
		{
			isDisplayNameTooLarge = true;
		}
		else
		{
			isDisplayNameTooLarge = false;
		}

		

		if (!isChangingDisplayName && !isDisplayNameTooLarge)
		{
			isChangingDisplayName = true;
			DWORD threadid;
			HANDLE hdl;
			DisplayNameHolder* DNH = new DisplayNameHolder{ charDisplayName };
			if (isInitialised)
			{
				hdl = CreateThread(NULL, 0, ChangeDisplayNameThread, DNH, 0, &threadid);
			}
			else
			{
				hdl = CreateThread(NULL, 0, InitialiseUserThread, DNH, 0, &threadid);
			}
			hdl = CreateThread(NULL, 0, ChangingThread, 0, 0, &threadid);

		}
		else if (isDisplayNameTooLarge)
		{
			currentStatusChangingUser = LanguageFileInitialiser::charAllTextsInApplication[23];
			currentColourChangingUser = tryingToConnectColour;
		}
	}
}


int __stdcall wWinMain(HINSTANCE _instace, HINSTANCE _previousInstance, PWSTR _arguments, int commandShow)
{
	gui::CreateHWindow("Hello", "HaxelClass");
	gui::CreateDevice();
	gui::CreateImGui();

	

	CreateSocket();

	config::StartConfigs();

	currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[3];
	currentStatusChangingUser = LanguageFileInitialiser::allTextsInApplication[3];
	allTextsInChatRoom.emplace_back(LanguageFileInitialiser::allTextsInApplication[18]);
	allTextsInChatRoom.emplace_back(LanguageFileInitialiser::allTextsInApplication[19]);

	while (gui::exit)
	{
		gui::BeginRender();
		gui::Render();
		if (config::isChatroomSelected)
		{
			Chatroom();
		}
		else if (config::isUserSelected)
		{
			User();
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
