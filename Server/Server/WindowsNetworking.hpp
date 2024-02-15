#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include <queue>

// Application Headers

#include "BufferStandard.hpp"
#include "Chatroom.hpp"
#include "NetworkCalls.hpp"

struct RecieveHolder
{
	// Allows us to hold the data as well as the socket position.
	int socketPosition;
	char* buffer = new char[maxBufferSize];
};

struct PositionHolder
{
	int position1;
	int position2;
};

// Implementation of NetworkCalls in windows.
class WindowsNetworking : public NetworkCalls
{
private:
	const static int numberOfClients = 32;

	static SOCKET serverSocket;

	static SOCKET clientSockets[numberOfClients];
	static bool clientAccepted[numberOfClients];
	static bool clientRecieving[numberOfClients];
	static int currentMessagePosition[numberOfClients];

	static bool isListening;

	static bool isBinded;

	static bool inChatroom;

	static Chatroom chatroom;

	static std::wstring currentWideIp;
	static std::string currentIp;
	static int currentPort;

	static DWORD WINAPI AcceptThread(LPVOID param);
	static DWORD WINAPI ListenThread(LPVOID param);
	static DWORD WINAPI BindThread(LPVOID param);
	static DWORD WINAPI DisconnectThread(LPVOID param);

	// All possible receives
	static DWORD WINAPI ReceiveSendMessageThread(LPVOID param);
	static DWORD WINAPI ReceiveConnect(LPVOID param);
	static DWORD WINAPI ReceiveUserUpdateThread(LPVOID param);
	static DWORD WINAPI ReceiveDisconnect(LPVOID param);

	static DWORD WINAPI ReceiveThread(LPVOID param);
	static DWORD WINAPI ReceiveVerifyThread(LPVOID param);
	static DWORD WINAPI ReceiveClientsThread(LPVOID param);

	static bool isOnline();
	
public:
	WindowsNetworking();

	// Fundamental Procedures
	void CreateSocket() override;
	void CloseSocket() override;
	void Bind(char* _ip, int _port) override;
	void Listen() override;
	void Receive() override;
	Chatroom* GetChatroom() override;
	void OpenChatroom(char* _chatroomName, char* _chatroomPassword) override;
	void CloseChatroom() override;
	void Disconnect() override;

	// Class Based
	bool GetListeningStatus() override;
	bool GetBindStatus() override;
	bool GetChatroomStatus() override;
    const char* GetCurrentIp() override;
	int GetCurrentPort() override;
};