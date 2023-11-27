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
BufferSendMessage::BufferSendMessage(int _type, std::string& _message) : m_message(_message), BufferNormal(_type){}
std::string BufferSendMessage::GetMessageString() const{	return m_message;	}
