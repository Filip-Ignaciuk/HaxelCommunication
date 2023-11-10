#include "WindowsNetworking.hpp"

#include "Languages.hpp"

DWORD WINAPI WindowsNetworking::ConnectThread(LPVOID param)
{
	ConnectHolder* CH = (ConnectHolder*)param;
	const std::wstring ip = CH->ip;
	const int port = CH->port;

	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(port);
	InetPtonW(AF_INET, ip.c_str(), &service.sin_addr.S_un.S_addr);

	if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{

	}
	else
	{

	}
	return 0;
}

DWORD WindowsNetworking::TryToConnectThread(LPVOID param)
{
	//ConnectHolder* CH = (ConnectHolder*)param;
	//// Converting ip input to const wchar_t
	//std::string stringIp = CH->ip;
	//std::wstring wideIp = std::wstring(stringIp.begin(), stringIp.end());
	//PCWSTR ip = wideIp.c_str();
	//int port = std::stoi(CH->port);
	//delete CH;
	//
	//sockaddr_in service;
	//service.sin_family = AF_INET;
	//service.sin_port = htons(port);
	//InetPtonW(AF_INET, ip, &service.sin_addr.S_un.S_addr);
	//
	//int result = connect(clientSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service));
	//if (result)
	//{
	//	currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[6];
	//}
	//else
	//{
	//	currentConnectionStatus = LanguageFileInitialiser::allTextsInApplication[4];
	//
	//}
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

DWORD WINAPI WindowsNetworking::ReceiveThread(LPVOID param)
{
	return 0;
}

WindowsNetworking::WindowsNetworking()
{
	clientSocket = INVALID_SOCKET;
}


bool WindowsNetworking::CreateSocket()
{
	WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(version, &wsaData))
	{
		// WSAGetLastError()
		return false;
	}


	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
	{
		WSACleanup();
		return false;
	}

	return true;
}

bool WindowsNetworking::CloseSocket()
{
	if(closesocket(clientSocket))
	{
		
	}
	else
	{
		
	}
	WSACleanup();
	return true;
}

void WindowsNetworking::Connect(const std::string& _ip, const int _port)
{
	// Convert Ip to wide Ip
	std::wstring wideIp = std::wstring(_ip.begin(), _ip.end());
	ConnectHolder* CH = new ConnectHolder{ wideIp, _port };

	DWORD threadId;
	HANDLE handle;
	//handle = CreateThread(NULL, 0, ConnectThread, (LPVOID)CH, 0, &threadId);
}

void WindowsNetworking::SendText(const std::string& _message)
{
	// Allocate string on heap
	std::string* message = const_cast<std::string*>(&_message);
	DWORD threadId;
	HANDLE handle;
	//handle = CreateThread(NULL, 0, SendTextThread, (LPVOID)message, 0, &threadId);
}

void WindowsNetworking::UpdateUser()
{
	DWORD threadId;
	HANDLE handle;
	//handle = CreateThread(NULL, 0, UpdateUserThread, nullptr, 0, &threadId);
}

void WindowsNetworking::Receive()
{
	DWORD threadId;
	HANDLE handle;
	//handle = CreateThread(NULL, 0, UpdateUserThread, nullptr, 0, &threadId);
}


// Override
NetworkCalls::~NetworkCalls() = default;


bool NetworkCalls::CreateSocket()
{
	return CreateSocket();
}

bool NetworkCalls::CloseSocket()
{
	return CloseSocket();
}

void NetworkCalls::Connect(const std::string& _ip, const int _port)
{
	Connect(_ip, _port);
}

void NetworkCalls::SendText(const std::string& _message)
{
	SendText(_message);
}

void NetworkCalls::UpdateUser()
{
	UpdateUser();
}


void NetworkCalls::Receive()
{
	Receive();
}

