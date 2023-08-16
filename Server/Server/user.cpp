#include "user.hpp"

#include <vector>

std::vector<User> users;


User::User(std::string _displayName) : displayName(_displayName)

{
	id = users.size();
	users.emplace_back(*this);
}


int User::GetId() const
{
	return id;
}

std::string User::GetDisplayName() const
{
	return displayName;
}
