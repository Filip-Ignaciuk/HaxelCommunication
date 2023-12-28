#pragma once
#include <string>

// Application Headers
#include "Chatroom.hpp"



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

class BufferSendMessage : public BufferNormal
{
private:
	std::string m_message;
public:
	BufferSendMessage(std::string& _message);
	std::string& GetMessageObject();
};


class BufferServerSendMessage : public BufferNormal
{
private:
	int m_position;
	std::string m_message;
public:
	BufferServerSendMessage(int _userPosition, std::string& _message);
	std::string& GetMessageObject();
	int GetPositionObject();
};

class BufferConnect : public BufferNormal
{
private:
	std::string m_password;
public:
	BufferConnect(std::string& _password);
	std::string GetPassword() const;
};

class BufferServerConnect : public BufferNormal
{
private:
	int m_isAccepted; // 2 - Requires password, 1 - Accepted, 0 - Not accepted.
	std::string m_name;
public:
	BufferServerConnect(int _isAccepted, std::string& _chatroomName);
	int GetIsAccepted() const;
	std::string& GetChatroomName();
};

class BufferUpdateUser : public BufferNormal
{
private:
	User m_user;
public:
	BufferUpdateUser(User& _user);
	User& GetUser();
};

class BufferServerUpdateUser : public BufferNormal
{
private:
	User m_user;
public:
	BufferServerUpdateUser(User& _user);
	User& GetUser();
};

class BufferDisconnect : public BufferNormal
{
private:

public:
	BufferDisconnect();
};

class BufferServerDisconnect : public BufferNormal
{
private:

public:
	BufferServerDisconnect();
};


// Max Size
constexpr int maxBufferSize = sizeof(BufferServerConnect);