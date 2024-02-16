


#include "Chatroom.hpp"
Chatroom::Chatroom() = default;

Chatroom::Chatroom(std::string& _name) : m_name(_name) {	}

void Chatroom::AddMessage(int _userPosition, std::string& _message)
{

	messageBuilder.Reset();
	messageBuilder.AddMessage(_userPosition, _message, GetUser(_userPosition));
	//Change in accordance to the users settings
	messageBuilder.BuildFromStyle();
	m_messages.emplace_back(*messageBuilder.GetFinalMessage());
	m_numberOfMessages++;

}

void Chatroom::UpdateUser(int _oldUserPosition, User& _newUser)
{ 
	bool hasMessagesChanged = false;

	m_users[_oldUserPosition] = _newUser;

	for (Message& message : m_messages)
	{
		if (message.GetUserPosition() == _oldUserPosition)
		{
			hasMessagesChanged = true;
			break;
		}
	}

	if (hasMessagesChanged)
	{
		UpdateMessages();
	}

	//const Error updateUserError(LanguageFileInitialiser::charAllTextsInApplication[33], 1);
	//ErrorHandler::AddError(updateUserError);
}

void Chatroom::UpdateName(std::string& _name)
{
	m_name = _name;
}

void Chatroom::AddPassword(std::string& _password)
{
	m_hasPassword = true;
	m_password = _password;
}


User Chatroom::GetUser(int _position)
{
	if(_position != 77)
	{
		return m_users[_position];
	}
	return *m_clientUser;
}

std::string Chatroom::GetChatroomName() const { return m_name; }

std::vector<Message>& Chatroom::GetMessages() { return m_messages; }

int Chatroom::GetNumberOfMessages() const { return m_numberOfMessages; }

bool Chatroom::HasPassword() const { return m_hasPassword; }

std::string Chatroom::GetPassword() const { return m_password; }

MessageBuilder& Chatroom::GetMessageBuilder() { return messageBuilder; }

void Chatroom::UpdateMessages()
{
	for (Message& message : m_messages)
	{
		messageBuilder.Reset();
		std::string originalMessage = message.GetOriginalMessage();
		messageBuilder.AddMessage(message.GetUserPosition(), originalMessage, GetUser(message.GetUserPosition()));
		//Change in accordance to the users settings
		messageBuilder.BuildFromStyle();

		message = *messageBuilder.GetFinalMessage();
	}
}

void Chatroom::SetClientUser(User* _User)
{
	m_clientUser = _User;
}

void Chatroom::SetChatroomName(std::string& _chatroomName)
{
	m_name = _chatroomName;
}
