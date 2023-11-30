#pragma once
#include <string>

#include "User.hpp"


class Message
{
private:
	// Complete message with all attributes
	std::string m_completeMessage;
	// Have original unedited message so that we can change it on the fly.
	std::string m_message;
	// Date we need for the message
	User m_user;
public:
	Message();
	Message(const std::string& _message, User& _user);
	void AddToMessage(const std::string& _message);
	void ChangeMainMessage(const std::string& _message);
	void ChangeUser(User& _user);
	void DeleteCompleteMessage();

	std::string GetMessageMain() const;
	std::string GetMessageComplete() const;
	User GetUser() const;

};

class MessageBuilder
{
private:
	const inline static std::string weekdaysShort[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
	const inline static std::string weekdaysLong[7] = { "Monday", "Tuesday", "Wed", "Thu", "Fri", "Sat", "Sun" };
	Message* m_Message;
public:
	MessageBuilder();
	~MessageBuilder();
	void Reset();
	void AddMessage(const std::string& _message);
	void AddTime();
	void AddDateLong();
	void AddDateShort();
	void AddId();
	void AddDisplayName();
	const Message* GetFinalMessage() const;
};