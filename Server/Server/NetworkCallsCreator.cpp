#include "NerworkCallsCreator.hpp"

NetworkCalls* NetworkCallsCreator::CreateNetworkCalls() const
{
	return new NetworkCalls;
}

NetworkCallsCreator::~NetworkCallsCreator()
{
	
}
