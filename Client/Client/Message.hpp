#pragma once
#include <string>


class Message
{
private:
	// Complete message with all atributes
	std::string m_completeMessage;
	// Have original unedited message so that we can change it on the fly.
	std::string m_message;
public:
	Message();
	Message(const std::string& _message);
	void AddMessage(const std::string& _message);
	void HasContent() const;
};

class MessageBuilder
{
private:
	const inline static std::string weekdaysShort[7] = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
	const inline static std::string weekdaysLong[7] = { "Monday", "Tuesday", "Wed", "Thu", "Fri", "Sat", "Sun" };
	Message* m_mainMessage;
	Message* m_preMessage;
public:
	MessageBuilder();
	~MessageBuilder();
	void Reset();
	void AddMessage(const std::string& _message);
	void AddTime();
	void AddDateLong();
	void AddDateShort();
	void AddId(const std::string& _id);
	const Message* GetMessage() const;
};