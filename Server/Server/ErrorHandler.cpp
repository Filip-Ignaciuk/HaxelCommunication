#include "ErrorHandler.hpp"

bool ErrorHandler::isQueueInUse = false;
bool ErrorHandler::hasErrors = false;
std::queue<Error> ErrorHandler::m_errors;

void ErrorHandler::AddError(const Error& _error)
{
	while (isQueueInUse)
	{
		
	}
	isQueueInUse = true;
	hasErrors = true;
	m_errors.emplace(_error);
	isQueueInUse = false;
}

Error ErrorHandler::GetError()
{
	while (isQueueInUse)
	{

	}
	isQueueInUse = true;
	Error frontError =  m_errors.front();
	isQueueInUse = false;
	return frontError;

}

void ErrorHandler::DeleteError()
{
	while (isQueueInUse)
	{

	}
	isQueueInUse = true;
	m_errors.pop();
	if(!m_errors.size())
	{
		hasErrors = false;
	}
	isQueueInUse = false;

}


bool ErrorHandler::HasError()
{
	return hasErrors;
}

