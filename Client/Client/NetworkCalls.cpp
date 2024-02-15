#include "NetworkCalls.hpp"

void NetworkCalls::CreateSocket(){}
void NetworkCalls::CloseSocket(){}
void NetworkCalls::Connect(char* _ip, const int _port, char* _password) {}
void NetworkCalls::Disconnect(){}


void NetworkCalls::SendText(char* _message) {}
void NetworkCalls::UpdateUser(User& _user) {}
void NetworkCalls::Receive() {}

bool NetworkCalls::GetReceivingStatus() { return false; }
bool NetworkCalls::GetChatroomStatus() { return false; }
bool NetworkCalls::GetConnectedStatus() { return false; }
bool NetworkCalls::GetUpdatedUserStatus() { return false; }

Chatroom& NetworkCalls::GetChatroom() { return *new Chatroom(); }

NetworkCalls::~NetworkCalls() = default;
