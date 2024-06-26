#include "NetworkCalls.hpp"
#include "Chatroom.hpp"

void NetworkCalls::CreateSocket(){}
void NetworkCalls::CloseSocket(){}
void NetworkCalls::Bind(char* _ip, int _port){}
void NetworkCalls::Listen() {}
void NetworkCalls::Receive() {}
Chatroom* NetworkCalls::GetChatroom() { return nullptr; }
void NetworkCalls::OpenChatroom(char* _chatroomName, char* _chatroomPassword){}
void NetworkCalls::CloseChatroom(){}
void NetworkCalls::Disconnect(){}

bool NetworkCalls::GetListeningStatus() { return false; }
bool NetworkCalls::GetBindStatus() { return false; }
bool NetworkCalls::GetChatroomStatus() { return false; }
const char* NetworkCalls::GetCurrentIp() { return ""; }
int NetworkCalls::GetCurrentPort() { return 0; }



NetworkCalls::~NetworkCalls() = default;
