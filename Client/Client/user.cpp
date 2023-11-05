#include "user.hpp"

User::User() = default;

User::User(const std::string& _displayName, const std::string& _id, float _userColourR, float _userColourG, float _userColourB) : m_displayName(_displayName), m_id(_id), m_userColourR(_userColourR), m_userColourG(_userColourG), m_userColourB(_userColourB)
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

ColourHolder User::GetUserColour() const
{
	ColourHolder CH = { m_userColourR, m_userColourG, m_userColourB };
	return CH;
}


