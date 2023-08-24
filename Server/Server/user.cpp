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
