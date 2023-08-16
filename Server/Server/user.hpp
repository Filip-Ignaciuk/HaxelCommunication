
#pragma once
#include <string>

class IDGenerator
{
	static int GenerateID();
};

class User
{
private:
	int id;
	std::string displayName;

public:
	User(std::string _displayName);


	int GetId() const;
	std::string GetDisplayName() const;

};