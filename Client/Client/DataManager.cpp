#include "DataManager.hpp"
#include <fstream>
#include "json.hpp"
#include "Config.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"
#include "WindowsNetworking.hpp"


bool DataManager::LoadUser(std::string& _name, User* _user, std::vector<int>& _style)
{
	// We're checking if this file exists
	std::ifstream userFile(config::currentDirNormalised + "/Users/" + _name + ".json");

	std::string userName;
	float userColourX = 0.0f;
	float userColourY = 0.0f;
	float userColourZ = 0.0f;
	int numOfStyle;
	std::vector<int> style;
	if (userFile.is_open())
	{
		nlohmann::json jsonData = nlohmann::json::parse(userFile);
		auto rawNameData = jsonData.find("name");
		if(rawNameData != jsonData.end())
		{
			if(rawNameData.value().is_string())
			{
				userName = rawNameData.value();
			}
		}

		auto rawColourXData = jsonData.find("colourx");
		if (rawColourXData != jsonData.end())
		{
			if (rawColourXData.value().is_number_float())
			{
				userColourX = rawColourXData.value();
			}
		}

		auto rawColourYData = jsonData.find("coloury");
		if (rawColourYData != jsonData.end())
		{
			if (rawColourYData.value().is_number_float())
			{
				userColourY = rawColourYData.value();
			}
		}

		auto rawColourZData = jsonData.find("colourz");
		if (rawColourZData != jsonData.end())
		{
			if (rawColourZData.value().is_number_float())
			{
				userColourZ = rawColourZData.value();
			}
		}

		auto rawNumofstyleData = jsonData.find("numofstyle");
		if (rawNumofstyleData != jsonData.end())
		{
			if (rawNumofstyleData.value().is_number())
			{
				numOfStyle = rawNumofstyleData.value();
			}
		}

		auto rawStyleData = jsonData.find("style");
		if (rawStyleData != jsonData.end())
		{
			if (rawStyleData.value().is_array())
			{
				style = rawStyleData.value().get<std::vector<int>>();
			}
		}

		

		ImVec4 userColour(userColourX, userColourY, userColourZ, 1.0f);
		User* user = new User((char*)userName.c_str(), (char*)"77", userColour);
		delete _user;
		_user = user;


		return true;

	}

	userFile.close();
	Error userNonExistant("User already exists, want to override?", 1);
	ErrorHandler::AddError(userNonExistant);
	return false;
}



void DataManager::SaveUser(User& _user, std::vector<int>& _style)
{
	// We're checking if this file exists
	std::ifstream userFile(config::currentDirNormalised + "/Users/" + _user.GetDisplayName() + ".json");
	if (!userFile.is_open())
	{
		nlohmann::json jsonData;
		jsonData["name"] = _user.GetDisplayName();
		jsonData["colourx"] = _user.GetUserColour().x;
		jsonData["coloury"] = _user.GetUserColour().y;
		jsonData["colourz"] = _user.GetUserColour().z;
		jsonData["Style"] = _style;

		userFile.close();
		std::ofstream outUserFile(config::currentDirNormalised + "/Users/" + _user.GetDisplayName() + ".json");
		outUserFile << jsonData;
		outUserFile.close();
	}
	else
	{
		// TODO: Implement are you sure to override feature;


		nlohmann::json jsonData;
		jsonData["name"] = _user.GetDisplayName();
		jsonData["colourx"] = _user.GetUserColour().x;
		jsonData["coloury"] = _user.GetUserColour().y;
		jsonData["colourz"] = _user.GetUserColour().z;
		jsonData["Style"] = _style;

		userFile.close();

		std::ofstream outUserFile(config::currentDirNormalised + "/Users/" + _user.GetDisplayName() + ".json");
		outUserFile << jsonData;
		outUserFile.close();
	}
}
