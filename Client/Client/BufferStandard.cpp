#include "BufferStandard.hpp"

BufferNormal::BufferNormal() = default;

BufferNormal::BufferNormal(int _type) : m_type(_type)
{
	
}

BufferNormal::~BufferNormal() = default;


int BufferNormal::GetType() const
{
	return m_type;
}

// BufferSendMessage
BufferSendMessage::BufferSendMessage(int _type, Message& _message) :BufferNormal(_type), m_message(_message) {}
Message& BufferSendMessage::GetMessageObject() { return m_message; }

// BufferServerSendMessage
BufferServerSendMessage::BufferServerSendMessage(int _type, Message& _message) :BufferNormal(_type), m_message(_message) {}
Message& BufferServerSendMessage::GetMessageObject() { return m_message; }


// BufferConnect
BufferConnect::BufferConnect(int _type, std::string& _message) :BufferNormal(_type), m_password(_message) {}
std::string BufferConnect::GetPassword() const { return m_password; }

// BufferServerConnect
BufferServerConnect::BufferServerConnect(int _type, int _isAccepted, Chatroom& _chatroom) :BufferNormal(_type), m_isAccepted(_isAccepted), m_chatroom(_chatroom) {}
int BufferServerConnect::GetIsAccepted() const { return m_isAccepted; }
Chatroom& BufferServerConnect::GetChatroom() { return m_chatroom; }

// BufferUpdateUser
BufferUpdateUser::BufferUpdateUser(int _type, User& _user) :BufferNormal(_type), m_user(_user) {}
User& BufferUpdateUser::GetUser() { return m_user; }

// BufferServerUpdateUser
BufferServerUpdateUser::BufferServerUpdateUser(int _type, User& _user) :BufferNormal(_type), m_user(_user) {}
User& BufferServerUpdateUser::GetUser() { return m_user; }