#include "WindowsNetworking.hpp"
#include "Languages.hpp"
#include "BufferStandard.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"
#include <WS2tcpip.h>


SOCKET WindowsNetworking::serverSocket = INVALID_SOCKET;
SOCKET WindowsNetworking::clientSockets[32];
bool WindowsNetworking::clientRecieving[32];
bool WindowsNetworking::isListening = false;
bool WindowsNetworking::isBinded = false;
bool WindowsNetworking::inChatroom = false;
Chatroom WindowsNetworking::chatroom;

std::wstring WindowsNetworking::currentWideIp = L"";
std::string WindowsNetworking::currentIp = "";
int WindowsNetworking::currentPort = 0;

DWORD WINAPI WindowsNetworking::AcceptThread(LPVOID param)
{
	SOCKET acceptedSocket = accept(serverSocket, NULL, NULL);
	if (acceptedSocket == INVALID_SOCKET || acceptedSocket == 4294967295)
	{
		WSACleanup();
		isListening = false;
		return 0;
	}

	for (int i = 0; i < 32; i++)
	{
		if(clientSockets[i] == 0)
		{
			SOCKET& socket = clientSockets[i];
			socket = acceptedSocket;
			Error acceptedSocketNotification(LanguageFileInitialiser::allTextsInApplication[39], 3);
			ErrorHandler::AddError(acceptedSocketNotification);
			return 0;
		}
	}
	
	
}

DWORD WINAPI WindowsNetworking::ListenThread(LPVOID param)
{
	Error listeningNotification(LanguageFileInitialiser::allTextsInApplication[38], 3);
	ErrorHandler::AddError(listeningNotification);
	if(listen(serverSocket, 1) == SOCKET_ERROR)
	{
		Error listeningError("Error listening for clients", 0);
		ErrorHandler::AddError(listeningError);
	}
	CreateThread(nullptr, 0, AcceptThread, nullptr, 0, nullptr);
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
	BufferSendMessage* BNPtr = (BufferSendMessage*)param;
	chatroom.AddMessage(BNPtr->GetMessageObject());
	
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveConnect(LPVOID param)
{
	RecieveConnectHolder* NCHPtr = (RecieveConnectHolder*)param;
	int socketPosition = NCHPtr->socketPosition;
	BufferConnect bC = *NCHPtr->bufferConnect;
	delete NCHPtr;
	if(chatroom.HasPassword())
	{
		if(NCHPtr->bufferConnect->GetPassword() == chatroom.GetPassword())
		{
			clientRecieving[socketPosition] = false;
			Error error("Accepted User, correct password.", 3);
			ErrorHandler::AddError(error);
			delete NCHPtr;
			return 0;
		}
		else
		{
			shutdown(clientSockets[socketPosition], 2);
			clientSockets[socketPosition] = 0;
			clientRecieving[socketPosition] = false;
			Error error("Reject User, incorrect password.", 3);
			ErrorHandler::AddError(error);
			delete NCHPtr;
			return 0;

		}

	}
	BufferServerConnect BSC(true, chatroom);
	send(clientSockets[socketPosition], (char*)&BSC, sizeof(BufferNormal), 0);
	Error error("Accepted User.", 3);
	ErrorHandler::AddError(error);
	clientRecieving[NCHPtr->socketPosition] = false;
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveThread(LPVOID param)
{
	int socketPosition = (int)param;
	SOCKET& clientSocket = clientSockets[socketPosition];
	char* buffer = new char[sizeof(BufferServerConnect)];
	// Use the largest possible class, so that we can accomidate everything.
	int recievedBytes = recv(clientSocket, buffer, sizeof(BufferServerConnect), 0);
	BufferNormal* BH = (BufferNormal*)buffer;
	if (!BH->GetType())
	{

	}
	else if (BH->GetType() == 1)
	{
		// Message Buffer
		BufferSendMessage* BNPtr = (BufferSendMessage*)&buffer;
		CreateThread(nullptr, 0, ReceiveSendMessageThread, BNPtr, 0, nullptr);
	}
	else if (BH->GetType() == 3)
	{
		// Message Buffer
		BufferConnect* BCPtr = (BufferConnect*)&buffer;
		RecieveConnectHolder* NCHPtr = new RecieveConnectHolder() ;
		NCHPtr->socketPosition = socketPosition;
		NCHPtr->bufferConnect = BCPtr;
		CreateThread(nullptr, 0, ReceiveConnect, NCHPtr, 0, nullptr);
	}
	clientRecieving[socketPosition] = false;
	delete buffer;
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveClients(LPVOID param)
{
	for (int i = 0; i < 32; i++)
	{
		if (clientSockets[i] != 0 && !clientRecieving[i])
		{
			CreateThread(nullptr, 0, ReceiveThread, (LPVOID)i, 0, nullptr);
		}
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
		const Error creatingSocketError(LanguageFileInitialiser::charAllTextsInApplication[25], 0);
		ErrorHandler::AddError(creatingSocketError);
		// WSAGetLastError()
	}


	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET)
	{
		WSACleanup();
		const Error creatingSocketError(LanguageFileInitialiser::charAllTextsInApplication[26], 0);
		ErrorHandler::AddError(creatingSocketError);
	}

}

void WindowsNetworking::CloseSocket() 
{
	//int result = shutdown(serverSocket, 2);
	//int error = WSAGetLastError(); // 10057
	
	if(!closesocket(serverSocket))
	{
		const Error ClosingSocketError(LanguageFileInitialiser::charAllTextsInApplication[27], 0);
		ErrorHandler::AddError(ClosingSocketError);
	}
	isBinded = false;
	inChatroom = false;
	serverSocket = INVALID_SOCKET;
	for (int i = 0; i < 32; i++)
	{
		SOCKET& clientSocket = clientSockets[i];
		shutdown(clientSocket, 2);
		closesocket(clientSocket);
		clientSocket = INVALID_SOCKET;
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
	CreateThread(nullptr, 0, BindThread, nullptr, 0, nullptr);
}

void WindowsNetworking::Listen()
{
	isListening = true;
	CreateThread(nullptr, 0, ListenThread, nullptr, 0,nullptr);

}

void WindowsNetworking::Receive()
{
	CreateThread(nullptr, 0, ReceiveClients, nullptr, 0, nullptr);
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



// Class Based
bool WindowsNetworking::GetListeningStatus() { return isListening; }
bool WindowsNetworking::GetBindStatus() {	return isBinded;	}
bool WindowsNetworking::GetChatroomStatus() { return inChatroom; }
const char* WindowsNetworking::GetCurrentIp() { return currentIp.c_str(); }
int WindowsNetworking::GetCurrentPort() { return currentPort; }

