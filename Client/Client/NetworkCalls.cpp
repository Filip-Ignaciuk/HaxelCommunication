#include "NetworkCalls.hpp"
#include "Storage.hpp"

void NetworkCalls::CreateSocket(){}
void NetworkCalls::CloseSocket(){}
void NetworkCalls::Connect(const std::string& _ip, const int _port, std::string& _password) {}
void NetworkCalls::Disconnect(){}


void NetworkCalls::SendText(std::string _message) {}
void NetworkCalls::UpdateUser(User& _user) {}
void NetworkCalls::Receive() {}

bool NetworkCalls::GetReceivingStatus() { return false; }
bool NetworkCalls::GetChatroomStatus() { return false; }
bool NetworkCalls::GetConnectedStatus() { return false; }
bool NetworkCalls::GetUpdatedUserStatus() { return false; }

Chatroom& NetworkCalls::GetChatroom() { return emptyChatroom; }

NetworkCalls::~NetworkCalls() = default;
