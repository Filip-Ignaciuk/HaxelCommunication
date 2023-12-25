#include "NerworkCallsCreator.hpp"



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

void NetworkCallsCreator::Disconnect() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->Disconnect();
	delete networkCalls;
}

void NetworkCallsCreator::SendText(const std::string& _message) const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->SendText(_message);
	delete networkCalls;
}

void NetworkCallsCreator::UpdateUser() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->UpdateUser();
	delete networkCalls;
}

void NetworkCallsCreator::Receive() const
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	networkCalls->Receive();
	delete networkCalls;
}
bool NetworkCallsCreator::GetReceivingStatus()
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	bool result = networkCalls->GetReceivingStatus();
	delete networkCalls;
	return result;
}
bool NetworkCallsCreator::GetConnectionStatus()
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	bool result = networkCalls->GetConnectionStatus();
	delete networkCalls;
	return result;
}
bool NetworkCallsCreator::GetChatroomStatus()
{
	NetworkCalls* networkCalls = this->CreateNetworkCalls();
	bool result = networkCalls->GetChatroomStatus();
	delete networkCalls;
	return result;
}

NetworkCallsCreator::~NetworkCallsCreator()
{
	
}
