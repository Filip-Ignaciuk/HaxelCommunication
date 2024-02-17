#pragma once
#include "stdafx.h"
#include <WinSock2.h>


// Application Headers
#include "Chatroom.hpp"
#include "NetworkCalls.hpp"

struct ConnectHolder
{
	const std::wstring ip;
	const int port;
	std::string password;
};

// Implementation of NetworkCalls in windows.
class WindowsNetworking : public NetworkCalls
{
private:
	static SOCKET clientSocket;

	static bool isReceiving;

	static bool isConnected;

	static bool inChatroom;

	static bool hasUpdatedUser;

	static User currentUser;

	static Chatroom chatroom;

	static DWORD WINAPI ConnectThread(LPVOID param);
	static DWORD WINAPI DisconnectThread(LPVOID param);

	// Send
	static DWORD WINAPI SendTextThread(LPVOID param);
	static DWORD WINAPI UpdateUserThread(LPVOID param);

	// Receive
	static DWORD WINAPI ReceiveSendMessageThread(LPVOID param);
	static DWORD WINAPI ReceiveConnectThread(LPVOID param);
	static DWORD WINAPI ReceiveUserUpdateThread(LPVOID param);
	static DWORD WINAPI ReceiveServerDisconnectThread(LPVOID param);
	static DWORD WINAPI ReceiveServerChatroomUpdateThread(LPVOID param);


	static DWORD WINAPI ReceiveThread(LPVOID param);
	
	
public:
	WindowsNetworking();

	// Fundamental Procedures
	void CreateSocket() override;
	void CloseSocket() override;
	void Connect(char* _ip, int _port, char* _password) override;
	void Disconnect() override;

	// Main Procedures
	void SendText(char* _message) override;
	void UpdateUser(User& _user) override;
	void Receive() override;

	// Class Based
	bool GetReceivingStatus() override;
	bool GetChatroomStatus() override;
	bool GetConnectedStatus() override;
	bool GetUpdatedUserStatus() override;
	Chatroom& GetChatroom() override;

};