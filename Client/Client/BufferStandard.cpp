#include "BufferStandard.hpp"


BufferNormal::BufferNormal() : m_type(0)
{

}

BufferNormal::BufferNormal(int _type) : m_type(_type) {}

BufferNormal::~BufferNormal() = default;


int BufferNormal::GetType() const
{
	return m_type;
}

// BufferSendMessage
BufferSendMessage::BufferSendMessage(Message& _message) :BufferNormal(1), m_message(_message) {}
Message& BufferSendMessage::GetMessageObject() { return m_message; }

// BufferServerSendMessage
BufferServerSendMessage::BufferServerSendMessage(Message& _message) :BufferNormal(2), m_message(_message) {}
Message& BufferServerSendMessage::GetMessageObject() { return m_message; }


// BufferConnect
BufferConnect::BufferConnect(std::string& _message) :BufferNormal(3), m_password(_message) {}
std::string BufferConnect::GetPassword() const { return m_password; }

// BufferServerConnect
BufferServerConnect::BufferServerConnect(int _isAccepted, Chatroom& _chatroom) :BufferNormal(4), m_isAccepted(_isAccepted), m_chatroom(_chatroom) {}
int BufferServerConnect::GetIsAccepted() const { return m_isAccepted; }
Chatroom& BufferServerConnect::GetChatroom() { return m_chatroom; }

// BufferUpdateUser
BufferUpdateUser::BufferUpdateUser(User& _user) :BufferNormal(5), m_user(_user) {}
User& BufferUpdateUser::GetUser() { return m_user; }

// BufferServerUpdateUser
BufferServerUpdateUser::BufferServerUpdateUser(User& _user) :BufferNormal(6), m_user(_user) {}
User& BufferServerUpdateUser::GetUser() { return m_user; }