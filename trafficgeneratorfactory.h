/*
 *  trafficgeneratorfactory.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

/*
 *  routingpolicyfactory.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/23/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *	This is the object factory for all routing policy objects.
 */

#ifndef __TRAFFICGENERATORFACTORY_H__
#define __TRAFFICGENERATORFACTORY_H__

#include <string>
#include <map>
#include "trafficgenerator.h"

namespace PhoenixSim
{
	class TrafficGeneratorFactoryDemolisher;
	
	class TrafficGeneratorFactory
	{
	public:
		typedef TrafficGenerator* (*CreateTrafficGeneratorCallback)();
		
		bool RegisterTrafficGenerator(CreateTrafficGeneratorCallback createFunction, std::string identifier);
		bool UnregisterTrafficGenerator(std::string identifier);
		TrafficGenerator* CreateTrafficGenerator(std::string identifier);
		
		static TrafficGeneratorFactory* Instance();
	private:
		typedef std::map<std::string, CreateTrafficGeneratorCallback> CreateTrafficGeneratorMap;
		CreateTrafficGeneratorMap callbackFunctions;
		
		TrafficGeneratorFactory();
		TrafficGeneratorFactory(const TrafficGeneratorFactory&);
		~TrafficGeneratorFactory();
		
		static TrafficGeneratorFactory* instance;
		
		friend class TrafficGeneratorFactoryDemolisher;
		static TrafficGeneratorFactoryDemolisher demolisher;
	};
	
	class TrafficGeneratorFactoryDemolisher
	{
	public:
		TrafficGeneratorFactoryDemolisher();
		~TrafficGeneratorFactoryDemolisher();
		void SetInstancePointer(TrafficGeneratorFactory* p);
	private:
		TrafficGeneratorFactory* instance;
	};
}


#endif // __TRAFFICGENERATORFACTORY_H__