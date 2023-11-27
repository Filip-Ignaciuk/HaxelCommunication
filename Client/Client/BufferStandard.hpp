#pragma once
#include <string>

constexpr int bufferSize = 200;

class BufferNormal
{
private:
	int m_type;
public:
	BufferNormal();
	BufferNormal(int _type);
	virtual ~BufferNormal();
	int GetType() const;
};


class BufferSendMessage : public BufferNormal
{
private:
	std::string m_message;
public:
	BufferSendMessage(int _type, std::string& _message);
	std::string GetMessageString() const;

};
