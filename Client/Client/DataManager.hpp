#pragma once
#include <vector>

#include "User.hpp"


class DataManager
{
public:
	static bool LoadUser(std::string& _name, User* _user, std::vector<int>& _style);

	static void SaveUser(User& _user, std::vector<int>& _style);
};
