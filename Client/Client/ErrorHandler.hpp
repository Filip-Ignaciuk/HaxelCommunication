#pragma once
#include <vector>

#include "Error.hpp"

class ErrorHandler
{
private:
	static std::vector<Error> m_errors;
	static void DisplayMessage(const Error _error);

public:
	static void AddError();

};
