#include "WindowsNetworking.hpp"
#include "Languages.hpp"
#include "BufferStandard.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"

SOCKET WindowsNetworking::serverSocket = INVALID_SOCKET;
bool WindowsNetworking::isReceiving = false;
bool WindowsNetworking::isBinded = false;
bool WindowsNetworking::inChatroom = false;
Chatroom WindowsNetworking::chatroom;

std::wstring WindowsNetworking::currentWideIp = L"";
std::string WindowsNetworking::currentIp = "";
int WindowsNetworking::currentPort = 0;

DWORD WINAPI WindowsNetworking::ListenThread(LPVOID param)
{

	return 0;
}

DWORD WINAPI WindowsNetworking::BindThread(LPVOID param)
{
	PCWSTR pcIp = currentWideIp.c_str();
	sockaddr_in service;
	service.sin_family = AF_INET;
	service.sin_port = htons(currentPort);
	InetPtonW(AF_INET, pcIp, &service.sin_addr.S_un.S_addr);

	if (!bind(serverSocket, reinterpret_cast<SOCKADDR*>(&service), sizeof(service)))
	{
		// Successful
		isBinded = true;


	}
	else
	{
		// UnSuccessful
		const Error ConnectionFaliure(LanguageFileInitialiser::charAllTextsInApplication[25], 1);
		ErrorHandler::AddError(ConnectionFaliure);
		// Reset Ip and Port
		currentPort = 0;
		currentWideIp = L"";
		currentIp = "";
	}
	return 0;
	
}

DWORD WINAPI WindowsNetworking::DisconnectThread(LPVOID param)
{
	// Send disconnect message
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

DWORD WINAPI WindowsNetworking::ReceiveThread(LPVOID param)
{
	BufferNormal buffer;
	recv(serverSocket, (char*)&buffer, sizeof(BufferNormal), 0);
	isReceiving = false;
	
	if(!buffer.GetType())
	{
		return 0;
	}
	else if(buffer.GetType() == 1)
	{
		// BufferSendMessage
		BufferNormal* BNPtr = &buffer;
		DWORD threadId;
		HANDLE handle;
		handle = CreateThread(nullptr, 0, ReceiveSendMessageThread, BNPtr, 0, &threadId);
	}
	else if (buffer.GetType() == 3)
	{
		// BufferConnect
		BufferNormal* BNPtr = &buffer;
		DWORD threadId;
		HANDLE handle;
		handle = CreateThread(nullptr, 0, ReceiveSendMessageThread, BNPtr, 0, &threadId);
	}
	else if (buffer.GetType() == 5)
	{
		// BufferUpdateUser
		BufferNormal* BNPtr = &buffer;
		DWORD threadId;
		HANDLE handle;
		handle = CreateThread(nullptr, 0, ReceiveSendMessageThread, BNPtr, 0, &threadId);
	}
	else if (buffer.GetType() == 4)
	{

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
		const Error creatingSocketError(LanguageFileInitialiser::charAllTextsInApplication[26], 0);
		ErrorHandler::AddError(creatingSocketError);
		// WSAGetLastError()
	}


	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		WSACleanup();
		const Error creatingSocketError(LanguageFileInitialiser::charAllTextsInApplication[27], 0);
		ErrorHandler::AddError(creatingSocketError);
	}

}

void WindowsNetworking::CloseSocket() 
{
	if(!closesocket(serverSocket))
	{
		const Error ClosingSocketError(LanguageFileInitialiser::charAllTextsInApplication[28], 0);
		ErrorHandler::AddError(ClosingSocketError);
	}
	
	WSACleanup();
}

void WindowsNetworking::Bind(const std::string& _ip, int _port) 
{
	// Convert Ip to wide Ip
	std::wstring wideIp = std::wstring(_ip.begin(), _ip.end());
	currentWideIp = wideIp;
	currentIp = _ip;
	currentPort = _port;
	DWORD threadId;
	HANDLE handle;
	handle = CreateThread(nullptr, 0, BindThread, nullptr, 0, &threadId);
}

Chatroom* WindowsNetworking::GetChatroom()
{
	return &chatroom;
}

void WindowsNetworking::OpenChatroom()
{
	inChatroom = true;
	Chatroom emptyChatroom;
	chatroom = emptyChatroom;
}


void WindowsNetworking::CloseChatroom()
{
	Chatroom emptyChatroom;
	chatroom = emptyChatroom;
	inChatroom = false;
}


void WindowsNetworking::Disconnect()
{
	DWORD threadId;
	HANDLE handle;
	handle = CreateThread(nullptr, 0, DisconnectThread, nullptr, 0, &threadId);
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



// Class Based
bool WindowsNetworking::GetReceivingStatus() { return isReceiving; }
bool WindowsNetworking::GetBindStatus() {	return isBinded;	}
bool WindowsNetworking::GetChatroomStatus() { return inChatroom; }
const char* WindowsNetworking::GetCurrentIp() { return currentIp.c_str(); }
int WindowsNetworking::GetCurrentPort() { return currentPort; }

