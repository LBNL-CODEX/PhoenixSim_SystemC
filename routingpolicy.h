/*
 *  routingpolicy.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/23/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ROUTINGPOLICY_H__
#define __ROUTINGPOLICY_H__

#include <string>
#include <vector>

#include "electronicmessage.h"

namespace PhoenixSim
{	
	class RoutingParameters
	{
		public:
			int routerId;
	};
	
	class RoutingPolicy
	{
		public:
			virtual void SetRoutingParameters(RoutingParameters *p_parameters) = 0;
			virtual std::pair<int, int> Route(ElectronicMessage* currentMessage, int inputPort, int inputVirtualChannel, const std::vector<int> &routerCredits) = 0;
			//virtual bool CheckCredits(ElectronicMessage* currentMessage, int inputPort, int outputPort, int inputVirtualChannel, int outputVirtualChannel);
		private:
	};
}

#endif // __ROUTINGPOLICY_H__

