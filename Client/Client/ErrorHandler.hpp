#pragma once
#include <vector>

#include "Error.hpp"

class ErrorHandler
{
private:
	static bool hasErrors;
	static std::vector<Error> m_errors;
	static void DisplayMessage(const Error _error);

public:
	static void AddError(const Error& _error);

};
