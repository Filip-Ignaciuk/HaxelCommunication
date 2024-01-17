#pragma once
#include "string"
#include "vector"

#include "Message.hpp"
#include "User.hpp"


class Chatroom
{
private:
	MessageBuilder messageBuilder;
	User* m_clientUser;
	User m_users[32];
	std::string m_name;
	std::vector<Message> m_messages;
	int m_numberOfMessages = 0;
	bool m_hasPassword = false;
	std::string m_password;
public:
	Chatroom();
	Chatroom(std::string& _name);

	// Add Elements
	void AddMessage(int _userPosition, std::string& _message);
	void UpdateUser(int _oldUserPosition, User& _newUser);
	void UpdateName(std::string& _name);
	void AddPassword(std::string& _password);

	// Get Elements
	User GetUser(int _position);
	std::string GetChatroomName() const;
	std::vector<Message>& GetMessages();
	int GetNumberOfMessages() const;
	bool HasPassword() const;
	std::string GetPassword() const;
	MessageBuilder& GetMessageBuilder();

	// Other
	void UpdateMessages();
	void SetClientUser(User* _User);

};
