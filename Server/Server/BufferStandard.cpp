#include "BufferStandard.hpp"


BufferNormal::BufferNormal() : m_type(0)
{
	
}

BufferNormal::BufferNormal(int _type) : m_type(_type){}

BufferNormal::~BufferNormal() = default;


int BufferNormal::GetType() const
{
	return m_type;
}

// BufferSendMessage
BufferSendMessage::BufferSendMessage(std::string& _message) :BufferNormal(1), m_message(_message) {}
std::string& BufferSendMessage::GetMessageObject() { return m_message; }

// BufferServerSendMessage
BufferServerSendMessage::BufferServerSendMessage(std::string& _message) :BufferNormal(2), m_message(_message) {}
std::string& BufferServerSendMessage::GetMessageObject() { return m_message; }


// BufferConnect
BufferConnect::BufferConnect(std::string& _message) :BufferNormal(3), m_password(_message) {}
std::string BufferConnect::GetPassword() const { return m_password; }

// BufferServerConnect
BufferServerConnect::BufferServerConnect(int _isAccepted, std::string& _chatroomName) :BufferNormal(4), m_isAccepted(_isAccepted), m_name(_chatroomName) {}
int BufferServerConnect::GetIsAccepted() const { return m_isAccepted; }
std::string& BufferServerConnect::GetChatroomName() { return m_name; }

// BufferUpdateUser
BufferUpdateUser::BufferUpdateUser(User& _user) :BufferNormal(5), m_user(_user) {}
User& BufferUpdateUser::GetUser() { return m_user; }

// BufferServerUpdateUser
BufferServerUpdateUser::BufferServerUpdateUser(User& _user) :BufferNormal(6), m_user(_user) {}
User& BufferServerUpdateUser::GetUser() { return m_user; }