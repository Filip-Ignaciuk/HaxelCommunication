#include "Error.hpp"


Error::Error() : m_message(""), m_level(0)
{
	
}

Error::Error(const std::string& _message, const int _level) : m_message(_message), m_level(_level)
{
	
}

Error::Error(const Error& _other) :m_message(_other.m_message), m_level(_other.m_level)
{
	
}

std::string Error::GetMessageString() const
{
	return m_message;
}

int Error::GetLevel() const
{
	return m_level;
}
