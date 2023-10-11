
#pragma once
#include <string>

struct ColourHolder
{
	float userColourX;
	float userColourY;
	float userColourZ;
};

class IDGenerator
{
	static int GenerateID();
};

class User
{
private:
	std::string id;
	std::string displayName;
	float userColourX;
	float userColourY;
	float userColourZ;

public:
	User();
	User(std::string _displayName, std::string _id, float _userColourX, float _userColourY, float _userColourZ);

	void SetId(std::string _id);
	std::string GetId() const;

	std::string GetDisplayName() const;
	void SetDisplayName(std::string _displayName);

	ColourHolder GetUserColour() const;

};