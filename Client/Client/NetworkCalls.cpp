#include "NetworkCalls.hpp"

void NetworkCalls::CreateSocket(){}
void NetworkCalls::CloseSocket(){}
void NetworkCalls::Connect(char* _ip, const int _port, char* _password) {}
void NetworkCalls::Disconnect(){}


void NetworkCalls::SendText(char* _message) {}
void NetworkCalls::UpdateUser(User* _user) {}
void NetworkCalls::Receive() {}

bool* NetworkCalls::GetReceivingStatus() { return nullptr; }
bool* NetworkCalls::GetChatroomStatus() { return nullptr; }
bool* NetworkCalls::GetConnectedStatus() { return nullptr; }
bool* NetworkCalls::GetUpdatedUserStatus() { return nullptr; }

Chatroom& NetworkCalls::GetChatroom() { return *new Chatroom(); }

NetworkCalls::~NetworkCalls() = default;
