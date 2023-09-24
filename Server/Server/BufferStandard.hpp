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

struct BufferRequestInitialiseServer
{
	// Required
	std::string requestedDomainName;
	std::string requestedDomainPassword;
	std::string requestedDomainAdminPassword;
	std::string requestedDomainIp;
	int requestedDomainPort;
	// Settings
	bool isDiscoverable;
};

struct BufferResponseInitialiseServer
{
	int response;
};

struct BufferRequestDeleteServer
{
	std::string requestedDomainName;
	std::string requestedDomainAdminPassword;
};

struct BufferResponseDeleteServer
{
	int response;
};