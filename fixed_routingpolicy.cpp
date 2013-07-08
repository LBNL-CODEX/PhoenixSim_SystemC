/*
 *  fixed_routingpolicy.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/23/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "fixed_routingpolicy.h"

using namespace std;
using namespace PhoenixSim;

void Fixed_RoutingPolicy::SetRouterId(int p_routerId)
{
}

void Fixed_RoutingPolicy::SetRoutingParameters(RoutingParameters *p_parameters)
{	
}

	pair<int, int> Fixed_RoutingPolicy::Route(ElectronicMessage* currentMessage, int inputPort, int inputVirtualChannel, const vector<int> &routerCredits)
{
	return make_pair(currentMessage->destinationAddress.GetAddress(0), 0);
}