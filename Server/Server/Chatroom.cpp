#include "Chatroom.hpp"

#include "ErrorHandler.hpp"
#include "Languages.hpp"

Chatroom::Chatroom() = default;

Chatroom::Chatroom(std::string& _name) : m_name(_name)	{	}

void Chatroom::AddUser(User& _user)
{
	for (User user : m_users)
	{
		
	}
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
		User messageUser = message.GetUser();
		if (messageUser == _oldUser)
		{
			message.ChangeUser(_newUser);
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

User Chatroom::GetUser(int _position) const	{	return m_users[_position];	}

std::vector<Message> Chatroom::GetMessages() const	{	return m_messages;	}

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
