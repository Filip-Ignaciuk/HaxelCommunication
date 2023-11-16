#include "ErrorHandler.hpp"

bool ErrorHandler::hasErrors = false;
std::queue<Error> ErrorHandler::m_errors;

void ErrorHandler::AddError(const Error& _error)
{
	hasErrors = true;
	m_errors.emplace(_error);
}

Error ErrorHandler::GetError()
{
	return m_errors.front();
}

void ErrorHandler::DeleteError()
{
	m_errors.pop();
}


bool ErrorHandler::HasError()
{
	return hasErrors;
}

