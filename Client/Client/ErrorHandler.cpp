#include "ErrorHandler.hpp"

bool ErrorHandler::hasErrors = false;
std::queue<Error> ErrorHandler::m_errors;

bool ErrorHandler::hasResponded = false;
Error ErrorHandler::responseError;

bool ErrorHandler::responseBool = false;
std::string ErrorHandler::responseString;

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

void ErrorHandler::SetHasResponded(bool _hasResponded)
{
	hasResponded = _hasResponded;
}

Error& ErrorHandler::GetCurrentResponseError()
{
	return responseError;
}

void ErrorHandler::SetCurrentResponseError(Error& _currentResponseError)
{
	responseError = _currentResponseError;
}

bool ErrorHandler::GetResponseBool()
{
	return responseBool;
}

void ErrorHandler::SetResponseBool(bool _responseBool)
{
	responseBool = _responseBool;
}

std::string& ErrorHandler::GetResponseString()
{
	return responseString;
}


void ErrorHandler::SetResponseString(std::string& _responseString)
{
	responseString = _responseString;
}



