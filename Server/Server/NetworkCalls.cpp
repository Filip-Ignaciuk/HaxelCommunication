#include "NetworkCalls.hpp"

void NetworkCalls::CreateSocket(){}
void NetworkCalls::CloseSocket(){}
void NetworkCalls::Bind(const std::string& _ip, int _port){}
void NetworkCalls::Disconnect(){}


void NetworkCalls::SendText(const std::string& _message) {}
void NetworkCalls::UpdateUser() {}
void NetworkCalls::Receive() {}

bool NetworkCalls::GetReceivingStatus() { return false; }
bool NetworkCalls::GetBindStatus() { return false; }
bool NetworkCalls::GetChatroomStatus() { return false; }
const char* NetworkCalls::GetCurrentIp() { return ""; }
int NetworkCalls::GetCurrentPort() { return 0; }



NetworkCalls::~NetworkCalls() = default;
