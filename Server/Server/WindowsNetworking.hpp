#pragma once
#include "stdafx.h"
#include <WinSock2.h>
#include <queue>

// Application Headers

#include "BufferStandard.hpp"
#include "Chatroom.hpp"
#include "NetworkCalls.hpp"

struct RecieveConnectHolder
{
	int socketPosition;
	BufferConnect* bufferConnect;
};

// Implementation of NetworkCalls in windows.
class WindowsNetworking : public NetworkCalls
{
private:
	static SOCKET serverSocket;

	static SOCKET clientSockets[32];
	static bool clientRecieving[32];
	static bool clientUpdated[32];
	static bool currentPosition[32];

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

	static DWORD WINAPI SendTextThread(LPVOID param);
	static DWORD WINAPI UpdateUserThread(LPVOID param);

	static DWORD WINAPI ReceiveSendMessageThread(LPVOID param);
	static DWORD WINAPI ReceiveConnect(LPVOID param);
	static DWORD WINAPI ReceiveUserUpdateThread(LPVOID param);
	static DWORD WINAPI ReceiveTextThread(LPVOID param);

	static DWORD WINAPI ReceiveClients(LPVOID param);
	static DWORD WINAPI ReceiveThread(LPVOID param);
	
	
public:
	WindowsNetworking();

	// Fundamental Procedures
	void CreateSocket() override;
	void CloseSocket() override;
	void Bind(const std::string& _ip, int _port) override;
	void Listen() override;
	void Receive() override;
	Chatroom* GetChatroom() override;
	void OpenChatroom() override;
	void CloseChatroom() override;
	void Disconnect() override;
	

	// Main Procedures
	void SendText(const std::string& _message) override;
	void UpdateUser() override;

	// Class Based
	bool GetListeningStatus() override;
	bool GetBindStatus() override;
	bool GetChatroomStatus() override;
    const char* GetCurrentIp() override;
	int GetCurrentPort() override;
};