/*
 *  fixed_routingpolicy.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/23/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __FIXED_ROUTINGPOLICY_H__
#define __FIXED_ROUTINGPOLICY_H__

#include <string>
#include "routingpolicy.h"
#include "routingpolicyfactory.h"

namespace PhoenixSim
{	
	class Fixed_RoutingParameters : public RoutingParameters
	{
		public:
	};
	
	class Fixed_RoutingPolicy : public RoutingPolicy
	{
		public:
			void SetRouterId(int p_routerId);
			void SetRoutingParameters(RoutingParameters *p_parameters);
			std::pair<int, int> Route(ElectronicMessage* currentMessage, int inputPort, int inputVirtualChannel, const std::vector<int> &routerCredits);
	};
	
	// Register with RoutingPolicyFactory
	namespace
	{
		RoutingPolicy* CreateFixedPolicy()
		{
			return new Fixed_RoutingPolicy;
		}
		const std::string identifier = "fixed";
		
		const bool registered = RoutingPolicyFactory::Instance()->RegisterRoutingPolicy(CreateFixedPolicy, identifier);
	}
}

#endif // __FIXED_ROUTINGPOLICY_H__