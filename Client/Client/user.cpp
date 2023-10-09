#include "user.hpp"

User::User() = default;

User::User(std::string _displayName, std::string _id, float* _userColour) : displayName(_displayName), id(_id), userColour(_userColour)
{

}


std::string User::GetId() const
{
	return id;
}

std::string User::GetDisplayName() const
{
	return displayName;
}

void User::SetDisplayName(std::string _displayName)
{
	displayName = std::move(_displayName);
}
