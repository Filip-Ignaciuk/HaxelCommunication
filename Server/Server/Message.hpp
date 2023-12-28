#pragma once
#include <string>

// Application Headers
#include "User.hpp"


class Message
{
private:
	// Complete message with all attributes
	std::string m_completeMessage;
	// Have original unedited message so that we can change it on the fly.
	std::string m_originalMessage;
	// Date we need for the message
	int m_userPosition;
public:
	Message(const std::string& _message, int _userPosition);
	void AddToMessage(const std::string& _message);
	void ChangeOriginalMessage(const std::string& _message);
	void ChangeUserPosition(int _userPosition);
	void DeleteCompleteMessage();

	std::string GetOriginalMessage() const;
	std::string GetMessageComplete() const;
	int GetUserPosition() const;

	Message& operator=(const Message& _other);

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
	void AddMessage(int _userPosition, std::string& _message);
	void AddMessage(Message& _message);
	void AddTime();
	void AddDateLong();
	void AddDateShort();
	void AddId();
	void AddDisplayName();
	Message* GetFinalMessage() const;
};