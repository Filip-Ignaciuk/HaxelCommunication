#include "WindowsNetworking.hpp"
#include "Languages.hpp"
#include "BufferStandard.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"

SOCKET WindowsNetworking::clientSocket = INVALID_SOCKET;
bool WindowsNetworking::isReceiving = false;
bool WindowsNetworking::isConnected = false;
bool WindowsNetworking::inChatroom = false;


DWORD WINAPI WindowsNetworking::ConnectThread(LPVOID param)
{
	ConnectHolder* CH = (ConnectHolder*)param;
	const std::wstring ip = CH->ip;
	const int port = CH->port;

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip.c_str(), &service.sin_addr.S_un.S_addr);

	if (!connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		// Successful
		isConnected = true;
		BufferConnect BC();

	}
	else
	{
		// UnSuccessful
		const Error ConnectionFaliure("Could not connect to desired ip. Check if your ip/port is correct.", 1);
		ErrorHandler::AddError(ConnectionFaliure);

	}
	return 0;
}

DWORD WINAPI WindowsNetworking::DisconnectThread(LPVOID param)
{
	// Send disconnect message

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

DWORD WINAPI WindowsNetworking::ReceiveThread(LPVOID param)
{
	BufferNormal buffer;
	recv(clientSocket, (char*)&buffer, sizeof(BufferNormal), 0);
	isReceiving = false;
	
	if(!buffer.GetType())
	{
		// Message Buffer
		BufferSendMessage& messageBuffer = dynamic_cast<BufferSendMessage&> (buffer);
		messageBuffer.GetMessageString();

	}
	else if(buffer.GetType() == 1)
	{
		// 
	}

	
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
	if(!closesocket(clientSocket))
	{
		const Error ClosingSocketError("Couldn't close socket.", 0);
		ErrorHandler::AddError(ClosingSocketError);
	}
	
	WSACleanup();
}

void WindowsNetworking::Connect(const std::string& _ip, int _port) 
{
	// Convert Ip to wide Ip
	std::wstring wideIp = std::wstring(_ip.begin(), _ip.end());
	ConnectHolder* CH = new ConnectHolder{ wideIp, _port };

	DWORD threadId;
	HANDLE handle;
	handle = CreateThread(nullptr, 0, ConnectThread, CH, 0, &threadId);
}

void WindowsNetworking::Disconnect()
{
	DWORD threadId;
	HANDLE handle;
	handle = CreateThread(nullptr, 0, ConnectThread, nullptr, 0, &threadId);
}


void WindowsNetworking::SendText(const std::string& _message) 
{
	// Allocate string on heap
	std::string* message = const_cast<std::string*>(&_message);
	DWORD threadId;
	HANDLE handle;
	handle = CreateThread(nullptr, 0, SendTextThread, message, 0, &threadId);
}

void WindowsNetworking::UpdateUser() 
{
	DWORD threadId;
	HANDLE handle;
	handle = CreateThread(nullptr, 0, UpdateUserThread, nullptr, 0, &threadId);
}

void WindowsNetworking::Receive()  
{
	// Declare isReceiving here so that the computer doesn't create multiple threads quickly.
	isReceiving = true;
	DWORD threadId;
	HANDLE handle;
	handle = CreateThread(nullptr, 0, ReceiveThread, nullptr, 0, &threadId);
}

bool WindowsNetworking::GetReceivingStatus() { return isReceiving; }
bool WindowsNetworking::GetConnectionStatus() {	return isConnected;	}
bool WindowsNetworking::GetChatroomStatus() { return inChatroom; }
