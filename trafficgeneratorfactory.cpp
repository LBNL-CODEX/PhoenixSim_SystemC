/*
 *  trafficgeneratorfactory.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "trafficgeneratorfactory.h"

#include <stdexcept>
#include <iostream>

using namespace std;
using namespace PhoenixSim;

TrafficGeneratorFactory* TrafficGeneratorFactory::instance = NULL;
TrafficGeneratorFactoryDemolisher TrafficGeneratorFactory::demolisher;

TrafficGeneratorFactory* TrafficGeneratorFactory::Instance()
{
	if(instance == NULL)
	{
		instance = new TrafficGeneratorFactory;
		demolisher.SetInstancePointer(instance);
	}
	return instance;
}

TrafficGeneratorFactory::TrafficGeneratorFactory()
{
}

TrafficGeneratorFactory::TrafficGeneratorFactory(const TrafficGeneratorFactory&)
{
}

TrafficGeneratorFactory::~TrafficGeneratorFactory()
{
}

bool TrafficGeneratorFactory::RegisterTrafficGenerator(CreateTrafficGeneratorCallback createFunction, string identifier)
{
	return callbackFunctions.insert( pair<string, CreateTrafficGeneratorCallback>(identifier, createFunction)).second;
}

bool TrafficGeneratorFactory::UnregisterTrafficGenerator(string identifier)
{
	return callbackFunctions.erase(identifier) == 1;
}

TrafficGenerator* TrafficGeneratorFactory::CreateTrafficGenerator(string identifier)
{
	CreateTrafficGeneratorMap::const_iterator it = callbackFunctions.find(identifier);
	if(it == callbackFunctions.end())
	{
		throw logic_error("in 'routingpolicyfactory.cpp' - Unknown routing policy identifier. (" + identifier + ")");
	}
	return (it->second)(); 
}

TrafficGeneratorFactoryDemolisher::TrafficGeneratorFactoryDemolisher()
{
}

TrafficGeneratorFactoryDemolisher::~TrafficGeneratorFactoryDemolisher()
{
	delete instance;
}

void TrafficGeneratorFactoryDemolisher::SetInstancePointer(TrafficGeneratorFactory* p)
{
	instance = p;
}
