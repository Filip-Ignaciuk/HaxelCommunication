#include "Chatroom.hpp"
#include "NerworkCallsCreator.hpp"


NetworkCallsCreator::~NetworkCallsCreator()
{
	
}

void NetworkCallsCreator::CreateSocket() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->CreateSocket();
	delete networkCalls;
}

void NetworkCallsCreator::CloseSocket() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->CloseSocket();
	delete networkCalls;
}

void NetworkCallsCreator::Bind(char* _ip, int _port)
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->Bind(_ip, _port);
	delete networkCalls;
}

void NetworkCallsCreator::Listen()
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->Listen();
	delete networkCalls;
}

void NetworkCallsCreator::Receive()
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->Receive();
	delete networkCalls;
}



Chatroom* NetworkCallsCreator::GetChatroom()
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
    Chatroom* result =	networkCalls->GetChatroom();
	delete networkCalls;
	return  result;
}

void NetworkCallsCreator::OpenChatroom(char* _chatroomName, char* _chatroomPassword)
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->OpenChatroom(_chatroomName, _chatroomPassword);
	delete networkCalls;
}


void NetworkCallsCreator::CloseChatroom()
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->CloseChatroom();
	delete networkCalls;
}

void NetworkCallsCreator::Disconnect() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->Disconnect();
	delete networkCalls;
}

bool NetworkCallsCreator::GetListeningStatus() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	bool result = networkCalls->GetListeningStatus();
	delete networkCalls;
	return result;
}

bool NetworkCallsCreator::GetBindStatus() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	bool result = networkCalls->GetBindStatus();
	delete networkCalls;
	return result;
}

bool NetworkCallsCreator::GetChatroomStatus() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	bool result = networkCalls->GetChatroomStatus();
	delete networkCalls;
	return result;
}

const char* NetworkCallsCreator::GetCurrentIp() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	const char* result = networkCalls->GetCurrentIp();
	delete networkCalls;
	return result;
}

int NetworkCallsCreator::GetCurrentPort() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	int result = networkCalls->GetCurrentPort();
	delete networkCalls;
	return result;
}