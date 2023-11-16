#pragma once
#include <string>

class Error
{
private:
	const std::string m_message;
	const int m_level;
public:
	Error();
	Error(const std::string& _message, const int _level);
	const std::string GetMessage() const;
	const int GetLevel() const;
};
