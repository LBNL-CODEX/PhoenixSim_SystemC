/*
 *  electronicfoldedtorus_xy_routingpolicy.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/5/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ELECTRONICFOLDEDTORUS_XY_ROUTINGPOLICY_H__
#define __ELECTRONICFOLDEDTORUS_XY_ROUTINGPOLICY_H__

#include <string>
#include "routingpolicy.h"
#include "routingpolicyfactory.h"

namespace PhoenixSim
{	
	class ElectronicFoldedTorus_XY_RoutingParameters : public RoutingParameters
	{
	public:
		int maxPacketSizeInCredits;
		int numOfVirtualChannels;
		int networkSizeX;
		int networkSizeY;
		int bufferWidth;
	};
	
	class ElectronicFoldedTorus_XY_RoutingPolicy : public RoutingPolicy
	{
	public:
		void SetRoutingParameters(RoutingParameters *p_parameters);
		std::pair<int, int> Route(ElectronicMessage* currentMessage, int inputPort, int inputVirtualChannel, const std::vector<int> &routerCredits);
		
	private:
		ElectronicFoldedTorus_XY_RoutingParameters routingParameters;
	};
	
	// Register with RoutingPolicyFactory
	namespace
	{
		RoutingPolicy* CreateElectronicFoldedTorusXYPolicy()
		{
			return new ElectronicFoldedTorus_XY_RoutingPolicy;
		}
		const std::string identifier_ft = "electronicfoldedtorus_xy";
		
		const bool registered_ft = RoutingPolicyFactory::Instance()->RegisterRoutingPolicy(CreateElectronicFoldedTorusXYPolicy, identifier_ft);
	}
}

#endif // __ELECTRONICFOLDEDTORUS_XY_ROUTINGPOLICY_H__