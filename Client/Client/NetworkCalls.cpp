#include "NetworkCalls.hpp"

bool NetworkCalls::CreateSocket()
{
	return false;
}

bool NetworkCalls::CloseSocket()
{
	return false;
}

void NetworkCalls::Connect(const std::string& _ip, const int _port) {}
void NetworkCalls::SendText(const std::string& _message) {}
void NetworkCalls::UpdateUser() {}
void NetworkCalls::Receive() {}

bool NetworkCalls::GetReceiving() { return false; }

NetworkCalls::~NetworkCalls() = default;
