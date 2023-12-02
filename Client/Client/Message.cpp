#include "Message.hpp"

#include <chrono>

#pragma warning(disable : 4996)

Message::Message(const std::string& _message, User& _user) : m_message(_message), m_user(_user) {  }

Message::Message(){  }

void Message::ChangeMainMessage(const std::string& _message)
{
	m_message = _message;
}

void Message::AddToMessage(const std::string& _message)
{
	m_completeMessage = m_completeMessage + _message;
}

void Message::ChangeUser(User& _user)
{
	m_user = _user;
}

void Message::DeleteCompleteMessage()
{
	m_completeMessage = "";
}


std::string Message::GetMessageMain() const
{
	return m_message;
}

std::string Message::GetMessageComplete() const
{
	return m_completeMessage;
}

User Message::GetUser() const
{
	return m_user;
}



MessageBuilder::MessageBuilder()
{
	this->Reset();
}

MessageBuilder::~MessageBuilder()
{
	delete m_Message;
}

void MessageBuilder::Reset()
{
	this->m_Message = new Message();
}

void MessageBuilder::AddMessage(const std::string& _message)
{
	this->m_Message->ChangeMainMessage(_message);
	
}

void MessageBuilder::AddTime()
{

	auto currentTime = std::chrono::system_clock::now();
	std::time_t finalTime = std::chrono::system_clock::to_time_t(currentTime);
	std::string stringTime = std::string(std::ctime(&finalTime));
	
	this->m_Message->AddToMessage(stringTime + " ");
}

void MessageBuilder::AddDateLong()
{
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto now_local = std::chrono::current_zone()->to_local(now);
	std::chrono::weekday today{ floor<std::chrono::days>(now_local) };
	this->m_Message->AddToMessage(weekdaysLong[today.c_encoding()] + " ");
}

void MessageBuilder::AddDateShort()
{
	std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
	auto now_local = std::chrono::current_zone()->to_local(now);
	std::chrono::weekday today{ floor<std::chrono::days>(now_local) };
	this->m_Message->AddToMessage(weekdaysShort[today.c_encoding()] + " ");
}
void MessageBuilder::AddId()
{
	this->m_Message->AddToMessage(m_Message->GetUser().GetId() +" ");
	
}

void MessageBuilder::AddDisplayName()
{
	this->m_Message->AddToMessage(m_Message->GetUser().GetDisplayName() + " ");
}


Message* MessageBuilder::GetFinalMessage() const
{
	this->m_Message->AddToMessage(">> " + m_Message->GetMessageMain());
	return m_Message;
}



