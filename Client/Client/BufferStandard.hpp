#pragma once
#include <string>

constexpr int bufferSize = 200;

struct BufferReady
{
	bool isReady;
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
