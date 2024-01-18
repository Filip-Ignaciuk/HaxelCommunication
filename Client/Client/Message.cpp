#include "Message.hpp"

#include <chrono>

#include "User.hpp"


#pragma warning(disable : 4996)

Message::Message(const std::string& _originalMessage, int _userPosition) : m_originalMessage(_originalMessage), m_userPosition(_userPosition) {  }

void Message::ChangeOriginalMessage(const std::string& _newOriginalMessage)
{
	m_originalMessage = _newOriginalMessage;
}

void Message::AddToMessage(const std::string& _message)
{
	m_completeMessage = m_completeMessage + _message;
}


void Message::ChangeUserPosition(int _userPosition)
{
	m_userPosition = _userPosition;
}

void Message::DeleteCompleteMessage()
{
	m_completeMessage = "";
}


std::string Message::GetOriginalMessage() const
{
	return m_originalMessage;
}

std::string Message::GetMessageComplete() const
{
	return m_completeMessage;
}

int Message::GetUserPosition() const
{
	return m_userPosition;
}

MessageBuilder::MessageBuilder()
{
	this->Reset();
}

MessageBuilder::~MessageBuilder()
{
	delete m_Message;
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
	// Lol
	this->m_Message->AddToMessage(m_currentUser.GetId() + " ");

}

void MessageBuilder::AddDisplayName()
{
	this->m_Message->AddToMessage(m_currentUser.GetDisplayName() + " ");
}

void MessageBuilder::Reset()
{
	this->m_Message = new Message("", 0);
}

void MessageBuilder::AddMessage(int _userPosition, std::string& _message, User _user)
{
	this->m_Message->ChangeOriginalMessage(_message);
	this->m_Message->ChangeUserPosition(_userPosition);
	this->m_currentUser = _user;
}

void MessageBuilder::AddMessage(Message& _message)
{
	this->m_Message->ChangeOriginalMessage(_message.GetOriginalMessage());
	this->m_Message->ChangeUserPosition(_message.GetUserPosition());
}

void MessageBuilder::BuildFromStyle()
{
	for (int num : style)
	{
		if(!num)
		{
			
		}
		else if(num == 1)
		{
			AddDisplayName();
		}
		else if (num == 2)
		{
			AddId();
		}
		else if (num == 3)
		{
			AddDateShort();
		}
		else if (num == 4)
		{
			AddDateLong();
		}
		else if (num == 5)
		{
			AddTime();
		}
	}
}

Message* MessageBuilder::GetFinalMessage() const
{
	this->m_Message->AddToMessage(">> " + m_Message->GetOriginalMessage());
	return m_Message;
}

std::vector<int>& MessageBuilder::GetStyle()
{
	return style;
}

void MessageBuilder::SetStyle(std::vector<int>& _style)
{
	style = _style;
}




Message& Message::operator=(const Message& _other)
{
	this->m_completeMessage = _other.m_completeMessage;
	this->m_originalMessage = _other.m_originalMessage;
	this->m_userPosition = _other.m_userPosition;
	return *this;
}

