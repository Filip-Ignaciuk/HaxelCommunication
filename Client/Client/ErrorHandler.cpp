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
	if(!m_errors.size())
	{
		hasErrors = false;
	}
}


bool ErrorHandler::HasError()
{
	return hasErrors;
}

bool ErrorHandler::GetHasResponded()
{
	return hasResponded;
}

bool ErrorHandler::SetHasResponded(bool _hasResponded)
{
	hasResponded = _hasResponded;
}

Error& ErrorHandler::GetCurrentResponseError()
{
	return responseError;
}

Error& ErrorHandler::SetCurrentResponseError(Error& _currentResponseError)
{
	responseError = _currentResponseError;
}

bool ErrorHandler::GetResponseBool()
{
	return responseBool;
}

bool ErrorHandler::SetResponseBool(bool _responseBool)
{
	responseBool = _responseBool;
}

std::string& ErrorHandler::GetResponseString()
{
	return responseString;
}


std::string& ErrorHandler::SetResponseString(std::string& _responseString)
{
	responseString = _responseString;




