#include "User.hpp"

User::User() = default;

User::User(const std::string& _displayName, const std::string& _id,ImVec4 _userColour) : m_displayName(_displayName), m_id(_id), m_UserColour(_userColour)
{

}

void User::SetId(const std::string& _id)
{
	m_id = _id;
}


std::string User::GetId() const
{
	return m_id;
}

std::string User::GetDisplayName() const
{
	return m_displayName;
}

void User::SetDisplayName(const std::string& _displayName)
{
	m_displayName = _displayName;
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
	if (_user1.m_id != _user2.m_id || _user1.m_displayName != _user2.m_displayName || _user1.m_UserColour.x != _user2.m_UserColour.x || _user1.m_UserColour.y != _user2.m_UserColour.y || _user1.m_UserColour.z != _user2.m_UserColour.z)
	{
		return false;
	}
	return true;
}

