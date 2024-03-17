#pragma once
#include "User.hpp"


class DataManager
{
public:
	static User LoadUser(std::string& _name);

	static void SaveUser(User& _user);
};
