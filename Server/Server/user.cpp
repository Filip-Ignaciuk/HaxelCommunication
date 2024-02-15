#include "User.hpp"

User::User() = default;

User::User(char* _displayName, char* _id,ImVec4 _userColour) : m_UserColour(_userColour)
{
	// Reallocate them on the heap so that the user name doesnt get changed externally.
	for (int i = 0; i < 128; i++)
	{
		m_displayName[i] = _displayName[i];
	}
	m_id[0] = _id[0];
	m_id[1] = _id[1];


}

void User::SetId(char* _id)
{
	m_id[0] = _id[0];
	m_id[1] = _id[1];
}
void User::SetDisplayName(char* _displayName)
{
	for (int i = 0; i < 128; i++)
	{
		m_displayName[i] = _displayName[i];
	}
}

char* User::GetId()
{
	return m_id;
}

char* User::GetDisplayName()
{
	return m_displayName;
}

ImVec4 User::GetUserColour() const
{
	return m_UserColour;
}

void User::SetUserColour(ImVec4& _newUserColour)
{
	m_UserColour = _newUserColour;
}


bool operator==(User& _user1, User& _user2)
{
	if (*_user1.m_id != *_user2.m_id || *_user1.m_displayName != *_user2.m_displayName || _user1.m_UserColour.x != _user2.m_UserColour.x || _user1.m_UserColour.y != _user2.m_UserColour.y || _user1.m_UserColour.z != _user2.m_UserColour.z)
	{
		return false;
	}
	return true;
}

