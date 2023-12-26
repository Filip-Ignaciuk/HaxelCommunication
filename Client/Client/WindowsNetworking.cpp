#include "WindowsNetworking.hpp"
#include "Languages.hpp"
#include "BufferStandard.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"
#include <WS2tcpip.h>

SOCKET WindowsNetworking::clientSocket = INVALID_SOCKET;
bool WindowsNetworking::isReceiving = false;
bool WindowsNetworking::inChatroom = false;
Chatroom WindowsNetworking::chatroom;


DWORD WINAPI WindowsNetworking::ConnectThread(LPVOID param)
{
	ConnectHolder* CH = (ConnectHolder*)param;
	const std::wstring ip = CH->ip;
	const int port = CH->port;
	std::string password = CH->password;

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip.c_str(), &service.sin_addr.S_un.S_addr);

	if (!connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		// Successful
		BufferConnect BC(password);
		send(clientSocket, (char*)&BC, sizeof(BufferConnect), 0);
	}
	else
	{
		// UnSuccessful
		const Error ConnectionFaliure("Could not connect to desired ip. Check if your ip/port is correct.", 1);
		ErrorHandler::AddError(ConnectionFaliure);

	}
	delete CH;
	return 0;
}

DWORD WINAPI WindowsNetworking::DisconnectThread(LPVOID param)
{
	// Send disconnect message
	BufferDisconnect BD;
	send(clientSocket, (char*)&BD, sizeof(BufferDisconnect), 0);
	shutdown(clientSocket, 2);
	if (!closesocket(clientSocket))
	{
		const Error ClosingSocketError("Couldn't close socket.", 0);
		ErrorHandler::AddError(ClosingSocketError);
	}

	WSACleanup();
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

	return 0;
}

DWORD WINAPI WindowsNetworking::SendTextThread(LPVOID param)
{
	std::string* message = (std::string*)param;



	return 0;
}

DWORD WINAPI WindowsNetworking::UpdateUserThread(LPVOID param)
{
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveSendMessageThread(LPVOID param)
{
	BufferNormal* bufferPointer = (BufferNormal*)param;
	BufferNormal bufferNormal = *bufferPointer;
	delete bufferPointer;
	BufferServerSendMessage& messageBuffer = dynamic_cast<BufferServerSendMessage&> (bufferNormal);
	chatroom.AddMessage(messageBuffer.GetMessageObject());
	
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveConnect(LPVOID param)
{
	BufferServerConnect* BSCPtr = (BufferServerConnect*)param;
	if(BSCPtr->GetIsAccepted())
	{
		chatroom = BSCPtr->GetChatroom();
		inChatroom = true;
	}
	else
	{
		Error incorrectPassword("Incorrect Password for chatroom", 2);
	}
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveThread(LPVOID param)
{
	isReceiving = true;
	char* buffer = new char[sizeof(BufferServerConnect)];
	// Use the largest possible class, so that we can accomidate everything.
	int recievedBytes = recv(clientSocket, buffer, sizeof(BufferServerConnect), 0);
	BufferNormal* BH = (BufferNormal*)buffer;
	if (!BH->GetType())
	{

	}
	else if (BH->GetType() == 2)
	{
		// BufferServerSendMessage
		BufferServerSendMessage* BSSMPtr = (BufferServerSendMessage*)&buffer;
		CreateThread(nullptr, 0, ReceiveSendMessageThread, BSSMPtr, 0, nullptr);
	}
	else if (BH->GetType() == 4)
	{
		// BufferConnectServer
		BufferServerConnect* BSCPtr = (BufferServerConnect*)&buffer;
		CreateThread(nullptr, 0, ReceiveConnect, BSCPtr, 0, nullptr);
	}
	isReceiving = false;
	delete buffer;
	return 0;
}


// Outwards Facing Functions

WindowsNetworking::WindowsNetworking()
{
}


void WindowsNetworking::CreateSocket()
{
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
	if (!closesocket(clientSocket))
	{
		const Error ClosingSocketError("Couldn't close socket.", 0);
		ErrorHandler::AddError(ClosingSocketError);
	}

	WSACleanup();
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


void WindowsNetworking::SendText(const std::string& _message) 
{
	// Allocate string on heap
	std::string* message = const_cast<std::string*>(&_message);
	CreateThread(nullptr, 0, SendTextThread, message, 0, nullptr);
}

void WindowsNetworking::UpdateUser() 
{
	CreateThread(nullptr, 0, UpdateUserThread, nullptr, 0, nullptr);
}

void WindowsNetworking::Receive()  
{
	// Declare isReceiving here so that the computer doesn't create multiple threads quickly.
	isReceiving = true;
	CreateThread(nullptr, 0, ReceiveThread, nullptr, 0, nullptr);
}

bool WindowsNetworking::GetReceivingStatus() { return isReceiving; }
bool WindowsNetworking::GetChatroomStatus() { return inChatroom; }
Chatroom& WindowsNetworking::GetChatroom() { return chatroom; }

