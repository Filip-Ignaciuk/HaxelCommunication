#include "Storage.hpp"
#include "ErrorHandler.hpp"
#include "Languages.hpp"


Chatroom::Chatroom() = default;

Chatroom::Chatroom(std::string& _name) : m_name(_name)	{	}

void Chatroom::AddUser(User& _user)
{
	
}

void Chatroom::AddMessage(Message& _message)
{
	

	messageBuilder.Reset();
	messageBuilder.AddMessage(_message);
	//Change in accordance to the users settings

	messageBuilder.GetFinalMessage();
	
}

void Chatroom::UpdateUser(User& _oldUser, User& _newUser)
{
	bool hasMessagesChanged = false;

	for (Message& message : m_messages)
	{
		int messageUser = message.GetUserPosition();
		if (messageUser == _oldUser)
		{
			message.ChangeUserPosition(_newUser);
			hasMessagesChanged = true;
		}
	}

	if (hasMessagesChanged)
	{
		UpdateMessages();
	}

	for (User& user : m_users)
	{
		if (user == _oldUser)
		{
			user = _newUser;
			return;
		}
		
	}

	const Error updateUserError(LanguageFileInitialiser::charAllTextsInApplication[33], 1);
	ErrorHandler::AddError(updateUserError);
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


User Chatroom::GetUser(int _position) const	{	return m_users[_position];	}

std::string& Chatroom::GetChatroomName() { return m_name; }

std::vector<Message> Chatroom::GetMessages() const	{	return m_messages;	}

bool Chatroom::HasPassword() const { return m_hasPassword; }

std::string& Chatroom::GetPassword() { return m_password; }



void Chatroom::UpdateMessages()
{
	for (Message& message : m_messages)
	{
		messageBuilder.Reset();
		messageBuilder.AddMessage(message);
		//Change in accordance to the users settings

		message = *messageBuilder.GetFinalMessage();


	}
}
