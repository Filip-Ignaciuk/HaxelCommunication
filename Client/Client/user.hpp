
#pragma once
#include <string>

class IDGenerator
{
	static int GenerateID();
};

class User
{
private:
	std::string id;
	std::string displayName;
	float userColour[3];

public:
	User();
	User(std::string _displayName, std::string _id, float _userColour[3]);

	std::string GetId() const;

	std::string GetDisplayName() const;
	void SetDisplayName(std::string _displayName);

	float* GetUserColour() const;

};