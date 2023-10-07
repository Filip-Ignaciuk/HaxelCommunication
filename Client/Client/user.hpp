#pragma once
#include <string>
#include "gui.h"	
#include "ImGui/imgui.h"

class IDGenerator
{
	static int GenerateID();
};

class User
{
private:
	std::string id;
	std::string displayName;
	

public:
	User();
	User(std::string _displayName, std::string _id);

	std::string GetId() const;
	std::string GetDisplayName() const;
	void SetDisplayName(std::string _displayName);

};