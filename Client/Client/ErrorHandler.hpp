#pragma once
#include <queue>

#include "Error.hpp"

class ErrorHandler
{
private:
	static bool hasErrors;
	static std::queue<Error> m_errors;
	
	// For Questions
	static bool hasResponded;
	static Error responseError;

	static bool responseBool;
	static std::string responseString;

public:
	static void AddError(const Error& _error);
	static Error GetError();
	static void DeleteError();
	static bool HasError();

	// For Questions
	static bool GetHasResponded();
	static bool SetHasResponded(bool _hasResponded);

	static Error& GetCurrentResponseError();
	static Error& SetCurrentResponseError(Error& _currentResponseError);

	static bool GetResponseBool();
	static bool SetResponseBool(bool _responseBool);
	
	static std::string& GetResponseString();
	static std::string& SetResponseString(std::string& _responseString);

	static void ClearCurrentResponse();
};
