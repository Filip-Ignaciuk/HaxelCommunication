#include "user.hpp"

#include <vector>

User::User() = default;

User::User(std::string _displayName, std::string _id) : displayName(_displayName), id(_id)
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

bool User::IsTheSame(User& user) const
{
	if (this->displayName == user.displayName && this->id == user.id)
	{
		return true;
	}
	return false;
}


bool User::operator==(User& user) const
{
	if(this->displayName == user.displayName && this->id == user.id)
	{
		return true;
	}
	return false;
}
