/*
 *  electronicmesh_xy_routingpolicy.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/24/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "electronicmesh_xy_routingpolicy.h"

using namespace std;
using namespace PhoenixSim;

void ElectronicMesh_XY_RoutingPolicy::SetRoutingParameters(RoutingParameters *p_parameters)
{	
	ElectronicMesh_XY_RoutingParameters* parameters = static_cast<ElectronicMesh_XY_RoutingParameters*>(p_parameters);
	
	routingParameters.routerId = parameters->routerId;
	routingParameters.networkSizeX = parameters->networkSizeX;
	routingParameters.networkSizeY = parameters->networkSizeY;
}

pair<int, int> ElectronicMesh_XY_RoutingPolicy::Route(ElectronicMessage* currentMessage, int inputPort, int inputVirtualChannel, const std::vector<int> &routerCredits)
{
	// This routing policy implements vanilla dimension ordered routing for a 2-D mesh.
	
	int routerLocationX = routingParameters.routerId % routingParameters.networkSizeX;
	int routerLocationY = routingParameters.routerId / routingParameters.networkSizeX;
	
	int destinationX = currentMessage->destinationAddress.GetAddress(1) % routingParameters.networkSizeX;
	int destinationY = currentMessage->destinationAddress.GetAddress(1) / routingParameters.networkSizeX;
	
	
	
	if(destinationX < routerLocationX)
	{
		return make_pair(3,0);
	}
	else if(destinationX > routerLocationX)
	{
		return make_pair(1,0);
	}
	else // destinationX == routerLocationX
	{
		if(destinationY < routerLocationY)
		{
			return make_pair(0,0);
		}
		else if(destinationY > routerLocationY)
		{
			return make_pair(2,0);
		}
		else // destinationY == routerLocationY
		{
			return make_pair(4 + currentMessage->destinationAddress.GetAddress(0),0);
		}
	}
}