#pragma once
#include <string>

// Application Headers
#include "Chatroom.hpp"

constexpr int bufferSize = 200;

/* The standard allows for objects to be sent back and forth between a client and a server.
 * Each class has a notion starting with Buffer, and then the person who is sending the object.
 * For example BufferConnect is a class reserved for the client to send an object to the server, and BufferServerConnect is the server's Response.
 *
 *
 */

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

class BufferConnect : public BufferNormal
{
private:
	std::string m_password;
public:
	BufferConnect(int _type, std::string& _password);
	std::string GetPassword() const;
};

class BufferServerConnect
{
private:
	int m_isAccepted; // 2 - Requires password, 1 - Accepted, 0 - Not accepted.
	Chatroom
public:
	
};

class BufferSendMessage : public BufferNormal
{
private:
	std::string m_message;
public:
	BufferSendMessage(int _type, std::string& _message);
	std::string GetMessageString() const;
};

