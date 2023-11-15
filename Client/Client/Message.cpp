#include "Message.hpp"

#include <chrono>

Message::Message(const std::string& _message) : m_message(_message) {  }

Message::Message(){  }

void Message::AddMessage(const std::string& _message)
{
	m_message = m_message + _message;
}


MessageBuilder::MessageBuilder()
{
	this->Reset();
}

MessageBuilder::~MessageBuilder()
{
	delete m_mainMessage;
	delete m_preMessage;
}

void MessageBuilder::Reset()
{
	this->m_mainMessage = new Message();
}

void MessageBuilder::AddMessage(const std::string& _message)
{
	this->m_mainMessage->AddMessage(_message);
	
}

void MessageBuilder::AddTime()
{

	auto currentTime = std::chrono::system_clock::now();
	std::time_t finalTime = std::chrono::system_clock::to_time_t(currentTime);
	std::string stringTime = std::string(std::ctime(&finalTime));

	if(hasContent)
	{
		
		this->m_preMessage->AddMessage(" " + stringTime + " ");
	}
	else
	{
		this->m_preMessage->AddMessage(stringTime + " ");
	}
}

void MessageBuilder::AddDate()
{
	
	 //if (hasContent)
	 //{
	 //	this->m_preMessage->AddMessage(" " + _id + " ");
	 //}
	 //else
	 //{
	 //	this->m_preMessage->AddMessage(_id + " ");
	 //}
}
void MessageBuilder::AddId(const std::string& _id)
{
	if(hasContent)
	{
		this->m_preMessage->AddMessage(" " + _id + " ");
	}
	else
	{
		this->m_preMessage->AddMessage(_id + " ");
	}
	
}



