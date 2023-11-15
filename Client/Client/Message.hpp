#pragma once
#include <string>


class Message
{
private:
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
	Message* m_mainMessage;
	Message* m_preMessage;
	bool hasContent;
public:
	MessageBuilder();
	~MessageBuilder();
	void Reset();
	void AddMessage(const std::string& _message);
	void AddTime();
	void AddDate();
	void AddId(const std::string& _id);
	const Message* GetMessage() const;
};