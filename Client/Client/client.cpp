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



const unsigned char numOfSentences = 28;

std::string allTextsInApplication[numOfSentences];
const char* charAllTextsInApplication[numOfSentences];

std::vector<std::string> allTextsInChatRoom;

User users[32];

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

//DWORD WINAPI TimedOutThread(LPVOID param)
//{
//	Sleep(25000);
//	isTimedOut = true;
//	currentConnectionStatus = allTextsInApplication[26];
//	currentColourConnection = failedToConnectColour;
//}
//void StartTimedOutThread()
//{
//	if(numOfTimedOutThreads != 3)
//	{
//		DWORD threadid;
//		timedOutHandles[numOfTimedOutThreads] = CreateThread(NULL, 0, TimedOutThread, 0, 0, &threadid);
//		numOfTimedOutThreads++;
//	}
//}
//void StopTimedOutThread()
//{
//	// ToDO
//	//TerminateThread();
//}

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
	std::string stringIp = CH->charIp;
	std::wstring wideIpInput = std::wstring(stringIp.begin(), stringIp.end());
	PCWSTR ip = wideIpInput.c_str();
	int port = std::stoi(CH->charPort);
	delete CH;

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		isConnecting = false;
		currentConnectionStatus = allTextsInApplication[6];
		currentColourConnection = failedToConnectColour;
	}
	else
	{
		isConnecting = false;
		isConnected = true;
		currentConnectionStatus = allTextsInApplication[4];
		currentColourConnection = connectedColour;
		
	}
	return 0;
	
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

struct ConnectHolderString
{
	std::string ip;
	std::string port;
};

DWORD WINAPI DomainThread(LPVOID param)
{
	ConnectHolder* CH = (ConnectHolder*)param;
	std::string domainName = CH->charIp;
	std::string domainPassword = CH->charPort;


	PCWSTR ip = L"127.0.0.1";
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(4096);
	InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);

	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{


		isConnecting = false;
		currentConnectionStatus = allTextsInApplication[28];
		currentColourConnection = failedToConnectColour;
	}
	else
	{
		// Testing
		//std::string bufferMaxSize = "R50B50BXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
		// int bytecount = sizeof(bufferMaxSize);
		std::string buffer = "R" + std::to_string(domainName.size()) + "B" + std::to_string(domainPassword.size()) + "B" + domainName + domainPassword;
		std::string resultBuffer = "";
		send(clientSocket, buffer.c_str(), bufferSize, 0);
		ConnectHolderString CHS;
		recv(clientSocket, (char*)&CHS, sizeof(ConnectHolderString), 0);
		if (CHS.port == "2")
		{
			isConnecting = false;
			currentConnectionStatus = allTextsInApplication[29];
			currentColourConnection = failedToConnectColour;
		}
		else
		{
			DWORD threadid;
			HANDLE hdl;
			ConnectHolder* CH2 = new ConnectHolder{ CHS.ip.data(), CHS.port.data() };
			hdl = CreateThread(NULL, 0, TryToConnectThread, CH2, 0, &threadid);

			
		}
		//else if(resultBuffer[0] == '1')
		//{
		//	isConnecting = false;
		//	currentConnectionStatus = allTextsInApplication[4];
		//	currentColourConnection = failedToConnectColour;
		//}
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
		if(gui::isTimeFormatOn)
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
			currentConnectionStatus = allTextsInApplication[25];
			currentColourChangingUser = notConnectedColour;
		}
	}
	else if (buffer[0] == 'Q')
	{
		if(buffer[1] == 'S')
		{
			currentConnectionStatus = allTextsInApplication[27];
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
	fileengb << "The server has been disconnected." << std::endl;
	fileengb << "Failed to connect to the domain server." << std::endl;
	fileengb << "Chatroom doesn't exist with chosen name." << std::endl;
	fileengb.close();

	std::ofstream filepl(currentDirNormalised + langWord + languages[1]);
	filepl << "IP" << std::endl;
	filepl << "Port" << std::endl;
	filepl << "Connection" << std::endl;
	filepl << "Nie połączony" << std::endl;
	filepl << "Połączony" << std::endl;
	filepl << "Połącz" << std::endl;
	filepl << "Nie udało się połączyć" << std::endl;
	filepl << "Łączenie." << std::endl;
	filepl << "Łączenie.." << std::endl;
	filepl << "Łączenie..." << std::endl;
	filepl << "Użytkownik" << std::endl;
	filepl << "Display Name" << std::endl;
	filepl << "Zmienianie." << std::endl;
	filepl << "Zmienianie.." << std::endl;
	filepl << "Zmienianie..." << std::endl;
	filepl << "Pomyślnie zainicjowano użytkownika" << std::endl;
	filepl << "Zaktualizuj użytkownika" << std::endl;
	filepl << "Czat" << std::endl;
	filepl << "W czacie" << std::endl;
	filepl << "Wpisz /h aby dostać pomoc" << std::endl;
	filepl << "Wiadomość" << std::endl;
	filepl << "Successfully changed user data" << std::endl;
	filepl << "Languages" << std::endl;
	filepl << "Display Name is too large" << std::endl;
	filepl << "Invalid port or IP" << std::endl;
	filepl << "Max clients within chatroom! Please wait for someone to leave or join another one." << std::endl;
	filepl << "Request timed out. Please try to reconnect to the server." << std::endl;
	filepl << "The server has been disconnected." << std::endl;
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


	bool isDisplayNameTooLarge = false;
	bool isPortOrIPValid = true;
	bool isMessageTooLong = false;
	bool isMessageTooShort = false;

	bool hasDomainNameFailed = false;


	int previousSize = 0;
	int previousLang = 0;
	int sizeOfMessage = 0;

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
		file << "1" << std::endl;

		language = 0;

		gui::currentLanguage = 0;
		gui::isTimeFormatOn = true;

		file.close();
		InitLanguageFiles();
	}
	else
	{
		std::string line;
		int i = 0;
		while(std::getline(file, line))
		{
			if (i == 1) { gui::currentLanguage = std::stoi(line); }
			else if (i == 2) { gui::isTimeFormatOn = std::stoi(line); }

			i++;
		}
		file.close();
	}

	isApplicationInitialised = true;

	InitialiseApplication();
	
	

	while (gui::exit)
	{
		char messageText[bufferSize] = "";
		gui::BeginRender();
		gui::Render();
		
		if(previousLang != gui::currentLanguage)
		{
			language = gui::currentLanguage;
			previousLang = language;
			InitialiseApplication();
		}
		

		ImGui::SeparatorText("Server");
		if(!isConnected)
		{
			// Ip
			ImGui::InputText(charAllTextsInApplication[0], charIp, IM_ARRAYSIZE(charIp));

			// Port
			ImGui::InputText(charAllTextsInApplication[1], charPort, IM_ARRAYSIZE(charPort));

			// Status
			ImGui::TextColored(currentColourConnection, currentConnectionStatus.c_str());

			if (!isPortOrIPValid && hasDomainNameFailed)
			{
				ImGui::Text(charAllTextsInApplication[24]);
			}

			if (ImGui::Button(charAllTextsInApplication[5]))
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

				if(!isPortOrIPValid)
				{
					isConnecting = true;
					DWORD threadid;
					HANDLE hdl;
					ConnectHolder* CH = new ConnectHolder{ charIp, charPort };
					hdl = CreateThread(NULL, 0, DomainThread, CH, 0, &threadid);
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
		if(isConnected)
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
							if(column)
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
							if(column)
							{
								const std::string userDisplayName = users[row].GetDisplayName();
								if(!userDisplayName.empty())
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
			if(ImGui::Button("Exit Chatroom."))
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
				recieveThread = CreateThread(NULL, 0, RecieveThread, 0, 0, &threadid);
			}

			ImGui::SeparatorText(charAllTextsInApplication[10]);
			if(isInitialised)
			{
				ImGui::Text(clientUser.GetDisplayName().c_str());
			}
			ImGui::InputText(charAllTextsInApplication[11], charDisplayName, IM_ARRAYSIZE(charDisplayName));
			ImGui::TextColored(currentColourChangingUser, currentStatusChangingUser.c_str());
			if(isDisplayNameTooLarge)
			{
				ImGui::Text(charAllTextsInApplication[23]);
			}
			if(ImGui::Button(charAllTextsInApplication[16]))
			{
				std::string name = charDisplayName;
				if(name.size() > 50)
				{
					isDisplayNameTooLarge = true;
				}
				else
				{
					isDisplayNameTooLarge = false;
				}
				
				if(!isChangingDisplayName && !isDisplayNameTooLarge)
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
				// Char doesn't work for some reason. Fix
				ImGui::SeparatorText(charAllTextsInApplication[17]);
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
				if(ImGui::InputText(charAllTextsInApplication[20], messageText, IM_ARRAYSIZE(messageText), ImGuiInputTextFlags_EnterReturnsTrue))
				{
					std::string stringMessage = messageText;
					sizeOfMessage = stringMessage.size();
					isMessageTooShort = false;
					isMessageTooLong = false;
					ImGui::SetKeyboardFocusHere(-1);
					if(messageText[0] == '/')
					{
						if(messageText[1] == 'h')
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

				if(isMessageTooLong)
				{
					ImGui::Text("Message too large. Consider splitting up your messages.");
				}
				else if(isMessageTooShort)
				{
					ImGui::Text("Message too small");
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
