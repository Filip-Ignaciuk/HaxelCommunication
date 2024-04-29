#include "DataManager.hpp"
#include <fstream>
#include "json.hpp"
#include "Config.hpp"
#include "Error.hpp"
#include "ErrorHandler.hpp"


User DataManager::LoadUser(std::string& _name)
{
	User user;
	// We're checking if this file exists
	std::ifstream userFile(config::currentDirNormalised + "/Users/" + _name + ".json");
	if (userFile.is_open())
	{
		nlohmann::json jsonData = nlohmann::json::parse(userFile);
		std::string userName = jsonData["name"].get<std::string>();
		float userColourX = jsonData["colourx"].get<float>();
		float userColourY = jsonData["coloury"].get<float>();
		float userColourZ = jsonData["colourz"].get<float>();
		//int numOfStyle = 

	}
	else
	{
		userFile.close();
		Error userNonExistant("User doesnt exist", 1);
		ErrorHandler::AddError(userNonExistant);
	}
	return user;
}



void DataManager::SaveUser(User& _user)
{
	// We're checking if this file exists
	std::ifstream userFile(config::currentDirNormalised + "/Users/" + _user.GetDisplayName() + ".json");
	if(!userFile.is_open())
	{
		userFile.close();
		std::ofstream userFile(config::currentDirNormalised + "/Users/" + _user.GetDisplayName() + ".json");


	}
	else
	{
		
	}
}
