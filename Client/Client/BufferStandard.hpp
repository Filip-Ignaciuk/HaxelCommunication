#pragma once
#include <string>

#include "user.hpp"

constexpr int bufferSize = 200;

struct BufferReady
{
	bool isReady;
};

struct BufferSendMessage
{
	std::string message;
	User user;
};

struct BufferRequestIp
{
	std::string requestedDomain;
	std::string requestedDomainPassword;
};

struct BufferResponseIp
{
	std::string responseIp;
	int responsePort;
};

struct BufferUpdateUser
{
	std::string displayName;
	
};
