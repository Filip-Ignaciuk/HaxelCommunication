#pragma once
#include <string>

// Application Headers
#include "Chatroom.hpp"
#include "Standard.hpp"


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
	char m_message[messageSize];
public:
	BufferSendMessage(char* _message);
	char* GetMessageObject();
};


class BufferServerSendMessage : public BufferNormal
{
private:
	int m_position;
	char m_message[messageSize];
public:
	BufferServerSendMessage(int _userPosition, char* _message);
	char* GetMessageObject();
	int GetPositionObject();
};

class BufferConnect : public BufferNormal
{
private:
	char m_password[wordSize];
public:
	BufferConnect(char* _password);
	char* GetPassword();
};

class BufferServerConnect : public BufferNormal
{
private:
	int m_isAccepted; // 2 - Requires password, 1 - Accepted, 0 - Not accepted.
	char m_name[wordSize];
public:
	BufferServerConnect(int _isAccepted, char* _chatroomName);
	int GetIsAccepted() const;
	char* GetChatroomName();
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
	int m_position;

public:
	BufferServerUpdateUser(User& _user, int _position);
	User& GetUser();
	int GetPosition() const;
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

// Have to send these in groups of 4, to be within under the maxBufferSize
//class BufferChatroomUpdate : public  BufferNormal
//{
//private:
//	int m_position; // 0 - 0th place in the array, 1 - 8th place in the array etc.
//	User m_users[8];
//public:
//	BufferServerChatroomUpdate(User* _users, int _position);
//	User& GetUser(int _position);
//	int GetPosition() const;
//};


// Have to send these in groups of 4, to be within under the maxBufferSize
class BufferServerChatroomUpdate : public  BufferNormal
{
private:
	int m_position; // 0 - 0th place in the array, 1 - 8th place in the array etc.
	User m_users[8];
public:
	BufferServerChatroomUpdate(User* _users, int _position);
	User& GetUser(int _position);
	int GetPosition() const;
};


// Max Size
constexpr int maxBufferSize = 2048;