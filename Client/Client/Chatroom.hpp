#pragma once
#include "string"
#include "vector"


#include "Message.hpp"
#include "User.hpp"

class Chatroom
{
private:
	static inline MessageBuilder messageBuilder;
	User m_users[32];
	std::string m_name;
	std::vector<Message> m_messages;
public:
	Chatroom();
	Chatroom(std::string& _name);

	// Add Elements
	void AddUser(User& _user);
	void AddMessage(Message& _message);
	void UpdateUser(User& _oldUser, User& _newUser);
	void UpdateName(std::string& _name);


	// Get Elements
	User GetUser(int _position) const;
	std::vector<Message> GetMessages() const;

	// Other
	void UpdateMessages();

};
