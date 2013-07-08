/*
 *  routingpolicyfactory.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/23/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include <stdexcept>
#include <iostream>
#include "routingpolicyfactory.h"

using namespace std;
using namespace PhoenixSim;

RoutingPolicyFactory* RoutingPolicyFactory::instance = NULL;
RoutingPolicyFactoryDemolisher RoutingPolicyFactory::demolisher;

RoutingPolicyFactory* RoutingPolicyFactory::Instance()
{
	if(instance == NULL)
	{
		instance = new RoutingPolicyFactory;
		demolisher.SetInstancePointer(instance);
	}
	return instance;
}

RoutingPolicyFactory::RoutingPolicyFactory()
{
}

RoutingPolicyFactory::RoutingPolicyFactory(const RoutingPolicyFactory&)
{
}

RoutingPolicyFactory::~RoutingPolicyFactory()
{
}

bool RoutingPolicyFactory::RegisterRoutingPolicy(CreateRoutingPolicyCallback createFunction, string identifier)
{
	return callbackFunctions.insert( pair<string, CreateRoutingPolicyCallback>(identifier, createFunction)).second;
}

bool RoutingPolicyFactory::UnregisterRoutingPolicy(string identifier)
{
	return callbackFunctions.erase(identifier) == 1;
}

RoutingPolicy* RoutingPolicyFactory::CreateRoutingPolicy(string identifier)
{
	CreateRoutingPolicyMap::const_iterator it = callbackFunctions.find(identifier);
	if(it == callbackFunctions.end())
	{
		throw logic_error("in 'routingpolicyfactory.cpp' - Unknown routing policy identifier. (" + identifier + ")");
	}
	return (it->second)(); 
}

RoutingPolicyFactoryDemolisher::RoutingPolicyFactoryDemolisher()
{
}

RoutingPolicyFactoryDemolisher::~RoutingPolicyFactoryDemolisher()
{
	delete instance;
}

void RoutingPolicyFactoryDemolisher::SetInstancePointer(RoutingPolicyFactory* p)
{
	instance = p;
}
