/*
 *  electronictorus_xy_routingpolicy.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/1/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ELECTRONICTORUS_XY_ROUTINGPOLICY_H__
#define __ELECTRONICTORUS_XY_ROUTINGPOLICY_H__

#include <string>
#include "routingpolicy.h"
#include "routingpolicyfactory.h"

namespace PhoenixSim
{	
	class ElectronicTorus_XY_RoutingParameters : public RoutingParameters
	{
	public:
		int maxPacketSizeInCredits;
		int numOfVirtualChannels;
		int networkSizeX;
		int networkSizeY;
		int bufferWidth;
	};
	
	class ElectronicTorus_XY_RoutingPolicy : public RoutingPolicy
	{
	public:
		void SetRoutingParameters(RoutingParameters *p_parameters);
		std::pair<int, int> Route(ElectronicMessage* currentMessage, int inputPort, int inputVirtualChannel, const std::vector<int> &routerCredits);
		
	private:
		ElectronicTorus_XY_RoutingParameters routingParameters;
	};
	
	// Register with RoutingPolicyFactory
	namespace
	{
		RoutingPolicy* CreateElectronicTorusXYPolicy()
		{
			return new ElectronicTorus_XY_RoutingPolicy;
		}
		const std::string identifier_t = "electronictorus_xy";
		
		const bool registered_t = RoutingPolicyFactory::Instance()->RegisterRoutingPolicy(CreateElectronicTorusXYPolicy, identifier_t);
	}
}

#endif // __ELECTRONICTORUS_XY_ROUTINGPOLICY_H__