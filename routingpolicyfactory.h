/*
 *  routingpolicyfactory.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/23/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *	This is the object factory for all routing policy objects.
 */

#ifndef __ROUTINGPOLICYFACTORY_H__
#define __ROUTINGPOLICYFACTORY_H__

#include <string>
#include <map>
#include "routingpolicy.h"

namespace PhoenixSim
{
	class RoutingPolicyFactoryDemolisher;
	
	class RoutingPolicyFactory
	{
		public:
			typedef RoutingPolicy* (*CreateRoutingPolicyCallback)();
		
			bool RegisterRoutingPolicy(CreateRoutingPolicyCallback createFunction, std::string identifier);
			bool UnregisterRoutingPolicy(std::string identifier);
			RoutingPolicy* CreateRoutingPolicy(std::string identifier);
		
			static RoutingPolicyFactory* Instance();
		private:
			typedef std::map<std::string, CreateRoutingPolicyCallback> CreateRoutingPolicyMap;
			CreateRoutingPolicyMap callbackFunctions;
		
			RoutingPolicyFactory();
			RoutingPolicyFactory(const RoutingPolicyFactory&);
			~RoutingPolicyFactory();
		
			static RoutingPolicyFactory* instance;
		
			friend class RoutingPolicyFactoryDemolisher;
			static RoutingPolicyFactoryDemolisher demolisher;
	};

	class RoutingPolicyFactoryDemolisher
	{
		public:
			RoutingPolicyFactoryDemolisher();
			~RoutingPolicyFactoryDemolisher();
			void SetInstancePointer(RoutingPolicyFactory* p);
		private:
			RoutingPolicyFactory* instance;
	};
}


#endif // __ROUTINGPOLICYFACTORY_H__

