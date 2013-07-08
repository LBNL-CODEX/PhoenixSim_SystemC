/*
 *  electronicfoldedtorus_xy_routingpolicy.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/5/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "electronicfoldedtorus_xy_routingpolicy.h"

using namespace std;
using namespace PhoenixSim;

void ElectronicFoldedTorus_XY_RoutingPolicy::SetRoutingParameters(RoutingParameters *p_parameters)
{	
	ElectronicFoldedTorus_XY_RoutingParameters* parameters = static_cast<ElectronicFoldedTorus_XY_RoutingParameters*>(p_parameters);
	
	
	routingParameters.maxPacketSizeInCredits = parameters->maxPacketSizeInCredits;
	routingParameters.numOfVirtualChannels = parameters->numOfVirtualChannels;
	routingParameters.routerId = parameters->routerId;
	routingParameters.networkSizeX = parameters->networkSizeX;
	routingParameters.networkSizeY = parameters->networkSizeY;
	routingParameters.bufferWidth = parameters->bufferWidth;
}

pair<int, int> ElectronicFoldedTorus_XY_RoutingPolicy::Route(ElectronicMessage* currentMessage, int inputPort, int inputVirtualChannel, const vector<int> &routerCredits)
{
	int routerLocationX = routingParameters.routerId % routingParameters.networkSizeX;
	int routerLocationY = routingParameters.routerId / routingParameters.networkSizeX;
	
	int destinationX = currentMessage->destinationAddress.GetAddress(1) % routingParameters.networkSizeX;
	int destinationY = currentMessage->destinationAddress.GetAddress(1) / routingParameters.networkSizeX;
	
	
	int leftDistance = (routerLocationX + routingParameters.networkSizeX - destinationX) % routingParameters.networkSizeX;
	int rightDistance = (destinationX + routingParameters.networkSizeX - routerLocationX) % routingParameters.networkSizeX;
	
	int upDistance = (routerLocationY + routingParameters.networkSizeY - destinationY) % routingParameters.networkSizeY;
	int downDistance = (destinationY + routingParameters.networkSizeY - routerLocationY) % routingParameters.networkSizeY;
	
	int outputPort = -1;
	int outputVirtualChannel = 0;
	
	if(destinationX == routerLocationX)
	{
		if(destinationY == routerLocationY)
		{
			outputPort = 4 + currentMessage->destinationAddress.GetAddress(0);
		}
		else 
		{
			if(upDistance < downDistance)
			{
				outputPort = 0;
			}
			else // upDistance >= downDistance
			{
				outputPort = 2;
			}	
		}
		
	}
	else
	{
		if(leftDistance < rightDistance)
		{
			outputPort = 3;
		}
		else // leftDistance >= rightDistance
		{
			outputPort = 1;
		}
	}
	
	// ensure bubble for dimension changes
	if(inputPort >= 4 || ((inputPort == 1 || inputPort == 3) && (outputPort == 0 || outputPort == 2)))
	{
		if(routerCredits[outputPort] - ceil(currentMessage->messageSize / double(routingParameters.bufferWidth)) < routingParameters.maxPacketSizeInCredits)
		{
			outputPort = -1;
		}
	}
	
	
	return make_pair(outputPort, outputVirtualChannel);

}