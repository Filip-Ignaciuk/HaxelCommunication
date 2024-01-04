#pragma once
#include <string>

#include "imgui.h"


struct IDGenerator
{
	static int GenerateID();
};

class User
{
private:
	std::string m_id;
	std::string m_displayName;
	ImVec4 m_UserColour;

public:
	User();
	User(const std::string& _displayName, const std::string& _id, ImVec4 _userColour);

	void SetId(const std::string& _id);
	std::string GetId() const;

	std::string GetDisplayName() const;
	void SetDisplayName(const std::string& _displayName);

	ImVec4 GetUserColour() const;
	void SetUserColour(ImVec4& _newUserColour);

	friend bool operator==(User& _user1, User& _user2);

};