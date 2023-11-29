#include "Chatroom.hpp"

Chatroom::Chatroom() = default;

Chatroom::Chatroom(std::string& _name) : m_name(_name)
{
	
}

void Chatroom::AddUser(User& _user)
{
	for (User user : m_users)
	{

	}
}

void Chatroom::AddMessage(Message& _message)
{
	
}

User Chatroom::GetUser(int _position) const
{
	return m_users[_position];
}

std::vector<Message> Chatroom::GetMessages() const
{
	return m_messages;
}
