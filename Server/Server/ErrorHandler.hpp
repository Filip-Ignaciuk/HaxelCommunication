#pragma once
#include <queue>

#include "Error.hpp"

class ErrorHandler
{
private:
	static bool hasErrors;
	static std::queue<Error> m_errors;

public:
	static void AddError(const Error& _error);
	static Error GetError();
	static void DeleteError();
	static bool HasError();
};
