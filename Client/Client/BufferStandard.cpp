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

// BufferConnect
BufferConnect::BufferConnect(int _type, std::string& _message) :BufferNormal(_type), m_password(_message) {}
std::string BufferConnect::GetPassword() const { return m_password; }

// BufferServerConnect
BufferServerConnect::BufferServerConnect(int _type, int _isAccepted, Chatroom& _chatroom) :BufferNormal(_type), m_isAccepted(_isAccepted), m_chatroom(_chatroom) {}
int BufferServerConnect::GetIsAccepted() const { return m_isAccepted; }
Chatroom& BufferServerConnect::GetChatroom() { return m_chatroom; }

// BufferSendMessage
BufferSendMessage::BufferSendMessage(int _type, std::string& _message) :BufferNormal(_type), m_message(_message) {}
std::string BufferSendMessage::GetMessageString() const { return m_message;	}
