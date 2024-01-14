#include "WindowsNetworking.hpp"
#include "Languages.hpp"
#include "BufferStandard.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"
#include <WS2tcpip.h>

SOCKET WindowsNetworking::clientSocket = INVALID_SOCKET;
bool WindowsNetworking::isReceiving = false;
bool WindowsNetworking::isConnected = false;
bool WindowsNetworking::inChatroom = false;
bool WindowsNetworking::hasUpdatedUser = false;
User WindowsNetworking::currentUser;
Chatroom WindowsNetworking::chatroom;


DWORD WINAPI WindowsNetworking::ConnectThread(LPVOID param)
{
	ConnectHolder* CH = (ConnectHolder*)param;
	const std::wstring ip = CH->ip;
	const int port = CH->port;
	std::string password = CH->password;
	
	// Send disconnect message

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip.c_str(), &service.sin_addr.S_un.S_addr);
	Chatroom emptyChatroom;
	chatroom = emptyChatroom;
	isReceiving = false;
	inChatroom = false;
	isConnected = false;
	

	if (!connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		// Successful
		BufferConnect BC(password);
		send(clientSocket, (char*)&BC, sizeof(BufferConnect), 0);
		isConnected = true;
	}
	else
	{
		// UnSuccessful
		int result = WSAGetLastError();
		const Error ConnectionFaliure("Could not connect to desired ip. Check if your ip/port is correct.", 1);
		ErrorHandler::AddError(ConnectionFaliure);

	}
	delete CH;
	return 0;
}

DWORD WINAPI WindowsNetworking::DisconnectThread(LPVOID param)
{
	// Send disconnect message
	if(inChatroom && isConnected)
	{
		BufferDisconnect BD;
		send(clientSocket, (char*)&BD, sizeof(BufferDisconnect), 0);
	}
	shutdown(clientSocket, 2);
	isConnected = false;
	isReceiving = false;
	inChatroom = false;

	return 0;
}

DWORD WINAPI WindowsNetworking::SendTextThread(LPVOID param)
{
	std::string message = *(std::string*)param;
	char* buffer = (char*)param;
	delete[] buffer;
	BufferSendMessage BSM(message);
	send(clientSocket, (char*)&BSM, sizeof(BufferSendMessage), 0);
	// We don't add it to our chatroom as the message could potentially fail.
	return 0;
}

DWORD WINAPI WindowsNetworking::UpdateUserThread(LPVOID param)
{
	if(inChatroom && isConnected)
	{
		BufferUpdateUser BUUPtr(currentUser);
		send(clientSocket, (char*)&BUUPtr, sizeof(BufferUpdateUser), 0);
		
	}

	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveSendMessageThread(LPVOID param)
{
	BufferServerSendMessage BSSM = *(BufferServerSendMessage*)param;
	char* buffer = (char*)param;
	delete[] buffer;

	chatroom.AddMessage(BSSM.GetPositionObject(), BSSM.GetMessageObject());
	
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveConnectThread(LPVOID param)
{
	BufferServerConnect BSC = *(BufferServerConnect*)param;
	char* buffer = (char*)param;
	delete[] buffer;

	if(BSC.GetIsAccepted())
	{
		chatroom = BSC.GetChatroomName();
		inChatroom = true;

		// Send User Information
		BufferUpdateUser BUUPtr(currentUser);
		send(clientSocket, (char*)&BUUPtr, sizeof(BufferUpdateUser), 0);
	}
	else
	{
		Error incorrectPassword("Incorrect Password for chatroom", 2);
		
	}
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveUserUpdateThread(LPVOID param)
{
	BufferServerUpdateUser BSUU = *(BufferServerUpdateUser*)param;
	char* buffer = (char*)param;
	delete[] buffer;
	if (BSUU.GetUser() == currentUser)
	{
		hasUpdatedUser = true;
	}
	chatroom.UpdateUser(BSUU.GetPosition(), BSUU.GetUser());

	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveServerDisconnectThread(LPVOID param)
{
	inChatroom = false;
	isConnected = false;
	isReceiving = false;
	shutdown(clientSocket, 2);
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveThread(LPVOID param)
{
	isReceiving = true;
	char* buffer = new char[sizeof(BufferServerUpdateUser)];
	// Use the largest possible class, so that we can accomidate everything.
	int recievedBytes = recv(clientSocket, buffer, sizeof(BufferServerUpdateUser), 0);
	BufferNormal BH = *(BufferNormal*)buffer;
	
	if (BH.GetType() == 2)
	{
		// BufferServerSendMessage
		CreateThread(nullptr, 0, ReceiveSendMessageThread, buffer, 0, nullptr);
	}
	else if (BH.GetType() == 4)
	{
		// BufferConnectServer
		CreateThread(nullptr, 0, ReceiveConnectThread, buffer, 0, nullptr);
	}
	else if (BH.GetType() == 6)
	{
		// BufferServerUpdateUser
		CreateThread(nullptr, 0, ReceiveUserUpdateThread, buffer, 0, nullptr);

	}
	else if (BH.GetType() == 6)
	{
		// BufferServerDisconnect
		delete[] buffer;
		CreateThread(nullptr, 0, ReceiveUserUpdateThread, buffer, 0, nullptr);

	}
	else
	{
		delete[] buffer;
	}
	isReceiving = false;
	return 0;
}


// Outwards Facing Functions

WindowsNetworking::WindowsNetworking()
{
}


void WindowsNetworking::CreateSocket()
{
	while (isConnected || inChatroom || isReceiving || clientSocket != INVALID_SOCKET)
	{

	}
	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(version, &wsaData))
	{
		const Error creatingSocketError("Couldn't create socket.", 0);
		ErrorHandler::AddError(creatingSocketError);
		// WSAGetLastError()
	}


	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		WSACleanup();
		const Error creatingSocketError("Couldn't create socket.", 0);
		ErrorHandler::AddError(creatingSocketError);
	}

}

void WindowsNetworking::CloseSocket()
{
	while (isConnected || inChatroom || isReceiving)
	{
		
	}
	if (clientSocket != INVALID_SOCKET)
	{
		if (closesocket(clientSocket))
		{
			const Error ClosingSocketError("Couldn't close socket.", 0);
			int result = WSAGetLastError();
			ErrorHandler::AddError(ClosingSocketError);
		}
		WSACleanup();
		clientSocket = INVALID_SOCKET;

	}

}

void WindowsNetworking::Connect(const std::string& _ip, int _port, std::string& _password) 
{
	// Convert Ip to wide Ip
	std::wstring wideIp = std::wstring(_ip.begin(), _ip.end());
	ConnectHolder* CH = new ConnectHolder{ wideIp, _port, _password};
	CreateThread(nullptr, 0, ConnectThread, CH, 0, nullptr);
}

void WindowsNetworking::Disconnect()
{
	CreateThread(nullptr, 0, DisconnectThread, nullptr, 0, nullptr);
}


void WindowsNetworking::SendText(std::string _message) 
{
	std::string* message = new std::string(_message);
	CreateThread(nullptr, 0, SendTextThread, message, 0, nullptr);
}

void WindowsNetworking::UpdateUser(User& _user) 
{
	currentUser = _user;
	CreateThread(nullptr, 0, UpdateUserThread, nullptr, 0, nullptr);
}

void WindowsNetworking::Receive()  
{
	if (!isReceiving && isConnected)
	{
		isReceiving = true;
		CreateThread(nullptr, 0, ReceiveThread, nullptr, 0, nullptr);
	}
	
}

bool WindowsNetworking::GetReceivingStatus() { return isReceiving; }
bool WindowsNetworking::GetChatroomStatus() { return inChatroom; }
bool WindowsNetworking::GetConnectedStatus() { return isConnected; }
bool WindowsNetworking::GetUpdatedUserStatus() { return hasUpdatedUser; }

Chatroom& WindowsNetworking::GetChatroom() { return chatroom; }

