#include "WindowsNetworking.hpp"
#include "Languages.hpp"
#include "BufferStandard.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"
#include <WS2tcpip.h>


SOCKET WindowsNetworking::serverSocket = INVALID_SOCKET;
SOCKET WindowsNetworking::clientSockets[32];
bool WindowsNetworking::clientAccepted[32];
bool WindowsNetworking::clientRecieving[32];
int WindowsNetworking::currentMessagePosition[32];
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

	return 0;
	
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
	int socketPosition = (int)param;
	SOCKET clientSocket = clientSockets[socketPosition];
	int& currentPos = currentMessagePosition[socketPosition];
	std::vector<Message>& messages = chatroom.GetMessages();
	while ((currentPos - 1) != chatroom.GetNumberOfMessages())
	{
		Message currentMessage = messages[currentPos];
		std::string originalMessage = currentMessage.GetOriginalMessage();
		BufferServerSendMessage BSSM(currentMessage.GetUserPosition(), originalMessage);
		send(clientSocket, (char*)&BSSM, sizeof(BufferServerSendMessage), 0);
		currentPos++;
	}

	return 0;
}

DWORD WINAPI WindowsNetworking::UpdateUserThread(LPVOID param)
{
	return 0;
}



DWORD WINAPI WindowsNetworking::ReceiveSendMessageThread(LPVOID param)
{
	RecieveHolder* NCHPtr = (RecieveHolder*)param;
	int socketPosition = NCHPtr->socketPosition;
	BufferSendMessage BC = *(BufferSendMessage*)NCHPtr->buffer;
	delete NCHPtr;
	chatroom.AddMessage(socketPosition, BC.GetMessageObject());

	
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveConnect(LPVOID param)
{
	RecieveHolder* NCHPtr = (RecieveHolder*)param;
	int socketPosition = NCHPtr->socketPosition;
	BufferConnect BC = *(BufferConnect*)NCHPtr->buffer;
	delete NCHPtr;
	std::string chatroomName = "";
	if(chatroom.HasPassword())
	{
		if(BC.GetPassword() == chatroom.GetPassword())
		{
			chatroomName = chatroom.GetChatroomName(); 
			BufferServerConnect BSC(true, chatroomName);
			send(clientSockets[socketPosition], (char*)&BSC, sizeof(BufferServerConnect), 0);
			clientRecieving[socketPosition] = false;
			Error error("Accepted User, correct password.", 3);
			ErrorHandler::AddError(error);
			clientAccepted[socketPosition] = true;
			return 0;
		}
		BufferServerConnect BSC(false, chatroomName);
		send(clientSockets[socketPosition], (char*)&BSC, sizeof(BufferServerConnect), 0);
		shutdown(clientSockets[socketPosition], 2);
		clientSockets[socketPosition] = 0;
		clientRecieving[socketPosition] = false;
		Error error("Reject User, incorrect password.", 3);
		ErrorHandler::AddError(error);
		return 0;

	}
	chatroomName = chatroom.GetChatroomName();
	BufferServerConnect BSC(true, chatroomName);
	send(clientSockets[socketPosition], (char*)&BSC, sizeof(BufferServerConnect), 0);
	Error error("Accepted User.", 3);
	ErrorHandler::AddError(error);
	clientRecieving[socketPosition] = false;
	clientAccepted[socketPosition] = true;
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveUserUpdateThread(LPVOID param)
{
	RecieveHolder* NCHPtr = (RecieveHolder*)param;
	int socketPosition = NCHPtr->socketPosition;
	BufferUpdateUser BC = *(BufferUpdateUser*)NCHPtr->buffer;
	delete NCHPtr;

	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveDisconnect(LPVOID param)
{
	RecieveHolder* NCHPtr = (RecieveHolder*)param;
	int socketPosition = NCHPtr->socketPosition;
	BufferDisconnect BC = *(BufferDisconnect*)NCHPtr->buffer;
	delete NCHPtr;
	
	Error error("User: " + chatroom.GetUser(socketPosition).GetDisplayName() + ", disconnected", 3);
	ErrorHandler::AddError(error);
	SOCKET& clientSocket = clientSockets[socketPosition];
	clientAccepted[socketPosition] = false;
	clientRecieving[socketPosition] = false;
	currentMessagePosition[socketPosition] = 0;
	shutdown(clientSocket, 2);
	closesocket(clientSocket);
	clientSocket = 0;
	
	
	return 0;
}


DWORD WINAPI WindowsNetworking::ReceiveThread(LPVOID param)
{
	int clientPosition = (int)param;
	SOCKET clientSocket = clientSockets[clientPosition];
	bool& clientRecievingStatus = clientRecieving[clientPosition];
	clientRecievingStatus = true;
	char* buffer = new char[maxBufferSize];
	// Use the largest possible class, so that we can accomidate everything.
	int recievedBytes = recv(clientSocket, buffer, sizeof(BufferServerConnect), 0);
	BufferNormal* BH = (BufferNormal*)buffer;
	RecieveHolder* RH = new RecieveHolder();
	RH->buffer = buffer;
	RH->socketPosition = clientPosition;

	if (BH->GetType() == 1)
	{
		// BufferSendMessage
		CreateThread(nullptr, 0, ReceiveSendMessageThread, RH, 0, nullptr);
	}
	else if (BH->GetType() == 3)
	{
		// BufferConnect
	}
	else if (BH->GetType() == 5)
	{
		// BufferUpdateUser

	}
	else if (BH->GetType() == 7)
	{
		// BufferDisconnect
		CreateThread(nullptr, 0, ReceiveDisconnect, RH, 0, nullptr);

	}
	else
	{
		// Invalid Buffer
		delete RH;
	}
	clientRecievingStatus = false;
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveVerifyThread(LPVOID param)
{
	int socketPosition = (int)param;
	char* buffer = new char[maxBufferSize];
	int recievedBytes = recv(clientSockets[socketPosition], buffer, sizeof(BufferServerConnect), 0);
	BufferNormal* BH = (BufferNormal*)buffer;
	if(BH->GetType() == 3)
	{
		RecieveHolder* RH = new RecieveHolder();
		RH->buffer = buffer;
		RH->socketPosition = socketPosition;
		CreateThread(nullptr, 0, ReceiveConnect, RH, 0, nullptr);
		return 0;
	}

	std::string emptyString = "";
	BufferServerConnect BSC(false, emptyString);
	send(clientSockets[socketPosition], (char*)&BSC, sizeof(BufferServerConnect), 0);
	shutdown(clientSockets[socketPosition], 2);
	clientSockets[socketPosition] = 0;
	clientRecieving[socketPosition] = false;
	Error error("Reject User, incorrect password.", 3);
	ErrorHandler::AddError(error);
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveClientsThread(LPVOID param)
{
	for (int i = 0; i < 32; i++)
	{
		if (clientSockets[i] != 0 && !clientRecieving[i] && clientAccepted[i])
		{
			CreateThread(nullptr, 0, ReceiveThread, (LPVOID)i, 0, nullptr);
		}
		else if (clientSockets[i] != 0 && !clientRecieving[i])
		{
			CreateThread(nullptr, 0, ReceiveVerifyThread, (LPVOID)i, 0, nullptr);
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
	CreateThread(nullptr, 0, ReceiveClientsThread, nullptr, 0, nullptr);
}


Chatroom* WindowsNetworking::GetChatroom()
{
	return &chatroom;
}

void WindowsNetworking::OpenChatroom(std::string& _chatroomName, std::string& _chatroomPassword)
{
	
	inChatroom = true;
	Chatroom emptyChatroom;
	chatroom = emptyChatroom;
	chatroom.UpdateName(_chatroomName);
	if (!_chatroomPassword.empty())
	{
		chatroom.AddPassword(_chatroomPassword);
	}
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


void WindowsNetworking::UpdateTexts()
{
	for (int i = 0; i < 32; i++)
	{
		if(clientSockets[i] != 0 && clientAccepted[i] && currentMessagePosition[i] < chatroom.GetNumberOfMessages())
		{
			CreateThread(nullptr, 0, SendTextThread, (LPVOID)i, 0, nullptr);
		}
	}
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

