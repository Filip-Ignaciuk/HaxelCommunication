#pragma once
#include <string>

class Error
{
private:
	std::string m_message;
	int m_level;
public:
	Error();
	Error(const std::string& _message, const int _level);
	Error(const Error& _other);
	std::string GetMessage() const;
	int GetLevel() const;
};
