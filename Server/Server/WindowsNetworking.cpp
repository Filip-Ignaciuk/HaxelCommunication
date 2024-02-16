#include "WindowsNetworking.hpp"
#include "Languages.hpp"
#include "BufferStandard.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"
#include <WS2tcpip.h>

SOCKET WindowsNetworking::serverSocket = INVALID_SOCKET;
SOCKET WindowsNetworking::clientSockets[numberOfClients];
bool WindowsNetworking::clientAccepted[numberOfClients];
bool WindowsNetworking::clientRecieving[numberOfClients];
int WindowsNetworking::currentMessagePosition[numberOfClients];
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
		isListening = false;
		return 0;
	}

	for (int i = 0; i < numberOfClients; i++)
	{
		if(clientSockets[i] == 0)
		{
			SOCKET& socket = clientSockets[i];
			socket = acceptedSocket;
			Error acceptedSocketNotification(LanguageFileInitialiser::allTextsInApplication[39], 3);
			ErrorHandler::AddError(acceptedSocketNotification);
			isListening = false;
			return 0;
		}
	}
	Error rejectedSocketNotification("Reached maximum capacity for clients", 3);
	ErrorHandler::AddError(rejectedSocketNotification);
	isListening = false;
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

DWORD WINAPI WindowsNetworking::ReceiveSendMessageThread(LPVOID param)
{
	RecieveHolder* NCHPtr = (RecieveHolder*)param;
	int socketPosition = NCHPtr->socketPosition;
	BufferSendMessage BC = *(BufferSendMessage*)NCHPtr->buffer;
	delete NCHPtr;

	std::string message = BC.GetMessageObject();

	chatroom.AddMessage(socketPosition, message);
	for (int i = 0; i < numberOfClients; i++)
	{
		if (clientSockets[i] != 0 && clientAccepted[i])
		{
			BufferServerSendMessage BSSM(socketPosition, BC.GetMessageObject());
			send(clientSockets[i], (char*)&BSSM, sizeof(BufferServerSendMessage), 0);
		}
	}

	Error recievedMessageNotification(BC.GetMessageObject(), 3);
	ErrorHandler::AddError(recievedMessageNotification);

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
		std::string userPassword = BC.GetPassword();
		std::string chatroomPassword = chatroom.GetPassword();
		bool test = userPassword == chatroomPassword;
		if(test)
		{
			chatroomName = chatroom.GetChatroomName();
			char chatroomNameArray[wordSize];
			const int chatroomNameSize = chatroomName.size();
			for (int i = 0; i < chatroomNameSize; i++)
			{
				chatroomNameArray[i] = chatroomName[i];
			}
			BufferServerConnect BSC(true, chatroomNameArray);
			send(clientSockets[socketPosition], (char*)&BSC, sizeof(BufferServerConnect), 0);
			Error error("Accepted User, correct password.", 3);
			ErrorHandler::AddError(error);
			//BufferServerChatroomUpdate* BSCUPtr = new BufferServerChatroomUpdate(chatroom.get);
			

			//send(clientSockets[socketPosition], (char*)buffer, sizeof(maxBufferSize), 0);

			clientAccepted[socketPosition] = true;

			
			return 0;
		}
		std::string empty = "";
		BufferServerConnect BSC(false, (char*)empty.c_str());
		send(clientSockets[socketPosition], (char*)&BSC, sizeof(BufferServerConnect), 0);
		shutdown(clientSockets[socketPosition], 2);
		clientSockets[socketPosition] = 0;
		clientRecieving[socketPosition] = false;
		Error error("Reject User, incorrect password.", 3);
		ErrorHandler::AddError(error);
		return 0;

	}
	chatroomName = chatroom.GetChatroomName();
	char chatroomNameArray[wordSize];
	const int chatroomNameSize = chatroomName.size();
	for (int i = 0; i < chatroomNameSize; i++)
	{
		chatroomNameArray[i] = chatroomName[i];
	}
	BufferServerConnect BSC(true, chatroomNameArray);
	send(clientSockets[socketPosition], (char*)&BSC, sizeof(BufferServerConnect), 0);
	Error error("Accepted User.", 3);
	ErrorHandler::AddError(error);

	//send(clientSockets[socketPosition], (char*)buffer, sizeof(maxBufferSize), 0);


	clientAccepted[socketPosition] = true;
	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveUserUpdateThread(LPVOID param)
{
	RecieveHolder* NCHPtr = (RecieveHolder*)param;
	int socketPosition = NCHPtr->socketPosition;
	BufferUpdateUser BC = *(BufferUpdateUser*)NCHPtr->buffer;
	delete NCHPtr;

	User user = BC.GetUser();
	char id[idWordSize];
	std::string stringId = std::to_string(socketPosition);
	id[0] = stringId[0];
	id[1] = stringId[1];
	user.SetId(id);

	chatroom.UpdateUser(socketPosition, user);
	BufferServerUpdateUser BSUU(user, socketPosition);

	for (int i = 0; i < numberOfClients; i++)
	{
		if (clientSockets[i] != 0 && clientAccepted[i])
		{
			send(clientSockets[i], (char*)&BSUU, sizeof(BufferServerUpdateUser), 0);
		}
	}

	

	return 0;
}

DWORD WINAPI WindowsNetworking::ReceiveDisconnect(LPVOID param)
{
	RecieveHolder* NCHPtr = (RecieveHolder*)param;
	int socketPosition = NCHPtr->socketPosition;
	BufferDisconnect BC = *(BufferDisconnect*)NCHPtr->buffer;
	delete NCHPtr;
	std::string displayName = chatroom.GetUser(socketPosition).GetDisplayName();
	Error error("User: " + displayName + ", disconnected", 3);
	ErrorHandler::AddError(error);
	User EmptyUser;
	chatroom.UpdateUser(socketPosition, EmptyUser);

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
	char* buffer = new char[sizeof(maxBufferSize)];
	// Use the largest possible class, so that we can accomidate everything.
	int recievedBytes = recv(clientSocket, buffer, sizeof(maxBufferSize), 0);
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
		delete RH;
	}
	else if (BH->GetType() == 5)
	{
		// BufferUpdateUser
		CreateThread(nullptr, 0, ReceiveUserUpdateThread, RH, 0, nullptr);

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

	char emptyArray[1];
	BufferServerConnect BSC(false, emptyArray);
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
	for (int i = 0; i < numberOfClients; i++)
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

bool WindowsNetworking::isOnline()
{
	if(isBinded && inChatroom)
	{
		return true;
	}
	return false;
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
		WSACleanup();
		const Error creatingSocketError(LanguageFileInitialiser::charAllTextsInApplication[25], 0);
		ErrorHandler::AddError(creatingSocketError);
		return;
	}
	

	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	int result = WSAGetLastError();
	if (serverSocket == INVALID_SOCKET)
	{
		WSACleanup();
		const Error creatingSocketError(LanguageFileInitialiser::charAllTextsInApplication[26], 0);
		ErrorHandler::AddError(creatingSocketError);
	}

}

void WindowsNetworking::CloseSocket() 
{
	isBinded = false;
	inChatroom = false;
	isListening = false;
	for (int i = 0; i < numberOfClients; i++)
	{
		if(clientSockets[i] != 0)
		{
			SOCKET& clientSocket = clientSockets[i];
			shutdown(clientSocket, 2);
			closesocket(clientSocket);
			clientSocket = INVALID_SOCKET;
		}
		clientAccepted[i] = false;
		clientRecieving[i] = false;
		
	}
	shutdown(serverSocket, 2);
	if(closesocket(serverSocket))
	{
		const Error ClosingSocketError(LanguageFileInitialiser::charAllTextsInApplication[27], 0);
		ErrorHandler::AddError(ClosingSocketError);
	}
	serverSocket = INVALID_SOCKET;
	WSACleanup();
}

void WindowsNetworking::Bind(char* _ip, int _port)
{
	// Convert Ip to wide Ip
	std::string ip = _ip;
	std::wstring wideIp = std::wstring(ip.begin(), ip.end());
	currentWideIp = wideIp;
	currentIp = _ip;
	currentPort = _port;
	CreateThread(nullptr, 0, BindThread, nullptr, 0, nullptr);
}

void WindowsNetworking::Listen()
{
	if (!isListening && inChatroom && isBinded)
	{
		isListening = true;
		CreateThread(nullptr, 0, ListenThread, nullptr, 0, nullptr);
	}

}

void WindowsNetworking::Receive()
{
	if(isBinded)
	{
		CreateThread(nullptr, 0, ReceiveClientsThread, nullptr, 0, nullptr);

	}
}


Chatroom* WindowsNetworking::GetChatroom()
{
	return &chatroom;
}

void WindowsNetworking::OpenChatroom(char* _chatroomName, char* _chatroomPassword)
{
	
	inChatroom = true;
	Chatroom emptyChatroom;
	chatroom = emptyChatroom;
	// This is rubbish i know
	std::string chatroomName;
	for (int i = 0; i < wordSize; i++)
	{
		char letter = _chatroomName[i];
		std::string letterS(1, letter);
		chatroomName.append(letterS);
	}
	chatroom.UpdateName(chatroomName);
	std::string chatroomPassword;
	for (int i = 0; i < wordSize; i++)
	{
		char letter = _chatroomPassword[i];
		std::string letterS(1, letter);
		chatroomPassword.append(letterS);
	}
	if (!chatroomPassword.empty())
	{
		chatroom.AddPassword(chatroomPassword);
	}
}


void WindowsNetworking::CloseChatroom()
{
	Disconnect();
	Chatroom emptyChatroom;
	chatroom = emptyChatroom;
	inChatroom = false;
}


void WindowsNetworking::Disconnect()
{
	CreateThread(nullptr, 0, DisconnectThread, nullptr, 0, nullptr);
}


// Class Based
bool WindowsNetworking::GetListeningStatus() { return isListening; }
bool WindowsNetworking::GetBindStatus() {	return isBinded;	}
bool WindowsNetworking::GetChatroomStatus() { return inChatroom; }
const char* WindowsNetworking::GetCurrentIp() { return currentIp.c_str(); }
int WindowsNetworking::GetCurrentPort() { return currentPort; }

