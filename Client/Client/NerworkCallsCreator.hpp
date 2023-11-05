#pragma once
#include "NetworkCalls.hpp"

class NetworkCallsCreator
{
public:
	virtual NetworkCalls* CreateNetworkCalls() const;
	virtual ~NetworkCallsCreator();
};