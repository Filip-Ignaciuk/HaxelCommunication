#include "BufferStandard.hpp"


BufferNormal::BufferNormal() : m_type(0)
{

}

BufferNormal::BufferNormal(int _type) : m_type(_type) {}

BufferNormal::~BufferNormal() = default;


int BufferNormal::GetType() const
{
	return m_type;
}

// BufferSendMessage
BufferSendMessage::BufferSendMessage(char* _message) :BufferNormal(1)
{
	for (int i = 0; i < messageSize; i++)
	{
		// Should implement more efficent algorithm here.
		m_message[i] = _message[i];
	}
}
char* BufferSendMessage::GetMessageObject() { return m_message; }

// BufferServerSendMessage
BufferServerSendMessage::BufferServerSendMessage(int _userPosition, char* _message) :BufferNormal(2), m_position(_userPosition)
{
	for (int i = 0; i < messageSize; i++)
	{
		// Should implement more efficent algorithm here.
		m_message[i] = _message[i];
	}
}
char* BufferServerSendMessage::GetMessageObject() { return m_message; }
int BufferServerSendMessage::GetPositionObject() { return m_position; }

// BufferConnect
BufferConnect::BufferConnect(char* _password) :BufferNormal(3)
{
	for (int i = 0; i < wordSize; i++)
	{
		m_password[i] = _password[i];
	}
}
char* BufferConnect::GetPassword() { return m_password; }

// BufferServerConnect
BufferServerConnect::BufferServerConnect(int _isAccepted, char* _chatroomName) :BufferNormal(4), m_isAccepted(_isAccepted)
{
	for (int i = 0; i < wordSize; i++)
	{
		m_name[i] = _chatroomName[i];
	}
}
int BufferServerConnect::GetIsAccepted() const { return m_isAccepted; }
char* BufferServerConnect::GetChatroomName() { return m_name; }

// BufferUpdateUser
BufferUpdateUser::BufferUpdateUser(User& _user) :BufferNormal(5), m_user(_user) {}
User& BufferUpdateUser::GetUser() { return m_user; }

// BufferServerUpdateUser
BufferServerUpdateUser::BufferServerUpdateUser(User& _user, int _position) :BufferNormal(6), m_user(_user), m_position(_position) {}
User& BufferServerUpdateUser::GetUser() { return m_user; }
int BufferServerUpdateUser::GetPosition() const { return m_position; }

// BufferDisconnect
BufferDisconnect::BufferDisconnect() : BufferNormal(7) {}

// BufferServerDisconnect
BufferServerDisconnect::BufferServerDisconnect() : BufferNormal(8) {}
