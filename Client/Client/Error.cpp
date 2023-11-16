#include "Error.hpp"


Error::Error() {}

Error::Error(const std::string& _message, const int _level) : m_message(_message), m_level(_level)
{
	
}

const std::string Error::GetMessage() const
{
	return m_message;
}

const int Error::GetLevel() const
{
	return m_level;
}
