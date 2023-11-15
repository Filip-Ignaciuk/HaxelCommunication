#include "ErrorHandler.hpp"

void ErrorHandler::DisplayMessage(const Error _error)
{
	
}

void ErrorHandler::AddError(const Error& _error)
{
	hasErrors = true;
	m_errors.emplace_back(_error);
}
