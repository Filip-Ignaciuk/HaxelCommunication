#include "user.hpp"

User::User() = default;

User::User(std::string _displayName, std::string _id, float _userColourX, float _userColourY, float _userColourZ) : displayName(_displayName), id(_id), userColourX(_userColourX), userColourY(_userColourY), userColourZ(_userColourZ)
{

}

void User::SetId(std::string _id)
{
	id = _id;
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

ColourHolder User::GetUserColour() const
{
	ColourHolder CH = { userColourX, userColourY, userColourZ };
	return CH;
}


