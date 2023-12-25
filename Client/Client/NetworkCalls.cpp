#include "NetworkCalls.hpp"

void NetworkCalls::CreateSocket(){}
void NetworkCalls::CloseSocket(){}
void NetworkCalls::Connect(const std::string& _ip, const int _port, std::string& _password) {}
void NetworkCalls::Disconnect(){}


void NetworkCalls::SendText(const std::string& _message) {}
void NetworkCalls::UpdateUser() {}
void NetworkCalls::Receive() {}

bool NetworkCalls::GetReceivingStatus() { return false; }
bool NetworkCalls::GetConnectionStatus() { return false; }
bool NetworkCalls::GetChatroomStatus() { return false; }

NetworkCalls::~NetworkCalls() = default;
