#pragma once
#include <string>

#include "imgui.h"
#include "Standard.hpp"

class User
{
private:
	char m_id[idWordSize];
	char m_displayName[wordSize];
	ImVec4 m_UserColour;

public:
	User();
	User(char* _displayName, char* _id, ImVec4 _userColour);

	void SetId(char* _id);
	void SetDisplayName(char* _displayName);


	char* GetId();
	char* GetDisplayName();

	ImVec4 GetUserColour() const;
	void SetUserColour(ImVec4& _newUserColour);

	friend bool operator==(User& _user1, User& _user2);

};