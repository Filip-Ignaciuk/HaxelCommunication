#include "Message.hpp"

#include <chrono>

#pragma warning(disable : 4996)

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
	
	this->m_preMessage->AddMessage(stringTime + " ");
}

void MessageBuilder::AddDateLong()
{
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto now_local = std::chrono::current_zone()->to_local(now);
	std::chrono::weekday today{ floor<std::chrono::days>(now_local) };
	this->m_preMessage->AddMessage(weekdaysLong[today.c_encoding()] + " ");
}

void MessageBuilder::AddDateShort()
{
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto now_local = std::chrono::current_zone()->to_local(now);
	std::chrono::weekday today{ floor<std::chrono::days>(now_local) };
	this->m_preMessage->AddMessage(weekdaysShort[today.c_encoding()] + " ");
}
void MessageBuilder::AddId(const std::string& _id)
{
	this->m_preMessage->AddMessage(_id + " ");
	
}



