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
	strcpy_s(m_message, _message);
}
char* BufferSendMessage::GetMessageObject() { return m_message; }

// BufferServerSendMessage
BufferServerSendMessage::BufferServerSendMessage(int _userPosition, char* _message) :BufferNormal(2), m_position(_userPosition)
{
	strcpy_s(m_message, _message);

}
char* BufferServerSendMessage::GetMessageObject() { return m_message; }
int BufferServerSendMessage::GetPositionObject() { return m_position; }

// BufferConnect
BufferConnect::BufferConnect(char* _password) :BufferNormal(3)
{
	strcpy_s(m_password, _password);

}
char* BufferConnect::GetPassword() { return m_password; }

// BufferServerConnect
BufferServerConnect::BufferServerConnect(int _isAccepted, char* _chatroomName) :BufferNormal(4), m_isAccepted(_isAccepted)
{
	strcpy_s(m_name, _chatroomName);
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

// BufferServerChatroomUpdate
BufferServerChatroomUpdate::BufferServerChatroomUpdate(User* _users, int _position) : BufferNormal(10), m_position(_position)
{
	for (size_t i = 0; i < 8; i++)
	{
		m_users[i] = _users[i];
	}
}
User& BufferServerChatroomUpdate::GetUser(int _position) { return m_users[_position]; }
int BufferServerChatroomUpdate::GetPosition() const { return m_position; }

