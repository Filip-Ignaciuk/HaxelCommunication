#pragma once
#include "string"
#include "vector"


#include "Message.hpp"
#include "User.hpp"

class Chatroom
{
private:
	User m_users[32];
	std::string m_name;
	std::vector<Message> m_messages;
public:
	Chatroom();
	Chatroom(std::string& _name);

	// Add Elements
	void AddUser(User& _user);
	void AddMessage(Message& _message);

	// Get Elements
	User GetUser(int _position) const;
	std::vector<Message> GetMessages() const;

};
