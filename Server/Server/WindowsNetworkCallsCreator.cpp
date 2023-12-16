#include "WindowsNetworkCallsCreator.hpp"
#include "WindowsNetworking.hpp"

NetworkCalls* WindowsCallsCreator::CreateNetworkCalls() const
{
	return new WindowsNetworking();
}
