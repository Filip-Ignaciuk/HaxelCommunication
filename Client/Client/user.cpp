#include "user.hpp"


User::User(std::string _displayName) : displayName(_displayName)
{

}


int User::GetId() const
{
	return id;
} 