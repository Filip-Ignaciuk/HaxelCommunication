#include <iostream>
#include <string>

class User
{
private:
	std::string m_username;
	std::string m_id;

public:
	User(std::string _m_username, const std::string _m_id) : m_username(_m_username), m_id(_m_id) {}
	std::string GetUsername() { return m_username; }
	std::string GetId() { return m_id; }
};


static class Interface
{
private:
	static std::string m_commandPrefix;

	static void SetCommandPrefix()
	{

	}
	static void SetCommandPrefix(const char _char)
	{

	}

public:
	static void DisplayText(User _user , std::string &_text)
	{
		std::cout << "(" << _user.GetId() << ") " << _user.GetUsername() << ": " << _text << std::endl;
	}

	static void InputText()
	{
		std::string input;
		std::cin >> input;
		std::cout << input;
	}
};


int main()
{
	Interface::InputText();	
}

