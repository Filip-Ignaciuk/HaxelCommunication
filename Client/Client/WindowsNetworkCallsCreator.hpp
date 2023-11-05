#pragma once

#include "NerworkCallsCreator.hpp"
#include "NetworkCalls.hpp"

class WindowsCallsCreator : public NetworkCallsCreator {
public:
    NetworkCalls* CreateNetworkCalls() const override;
};