
#pragma once
#include <string>

struct ColourHolder
{
	float colourR;
	float colourG;
	float colourB;
};

struct IDGenerator
{
	static int GenerateID();
};

class User
{
private:
	std::string m_id;
	std::string m_displayName;
	float m_userColourR;
	float m_userColourG;
	float m_userColourB;

public:
	User();
	User(const std::string& _displayName, const std::string& _id, const float _userColourR, const float _userColourG, const float _userColourB);

	void SetId(const std::string& _id);
	std::string GetId() const;

	std::string GetDisplayName() const;
	void SetDisplayName(const std::string& _displayName);

	ColourHolder GetUserColour() const;

	friend bool operator==(User& _user1, User& _user2);

};