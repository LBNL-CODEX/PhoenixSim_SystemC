/*
 *  random_trafficgenerator.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __RANDOM_TRAFFICGENERATOR_H__
#define __RANDOM_TRAFFICGENERATOR_H__


#include "systemc.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/exponential_distribution.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "trafficgenerator.h"
#include "trafficgeneratorfactory.h"
#include "electronicmessage.h"
#include "address.h"

namespace PhoenixSim
{
	class Random_TrafficGeneratorParameters: public TrafficGeneratorParameters
	{
		public:
			Random_TrafficGeneratorParameters();
		
			Address addressSpace;
			Address nodeAddress;
			sc_time interarrivalTime;
			int messageSize;
			int numOfMessages;
	};
	
	class Random_TrafficGenerator: public TrafficGenerator
	{
	public:
		
		Random_TrafficGenerator();
		~Random_TrafficGenerator();
		
		void SetTrafficGeneratorParameters(TrafficGeneratorParameters *p_parameters);
		
		virtual ElectronicMessage* CreateFirstMessage();
		virtual ElectronicMessage* MessageReceived(ElectronicMessage* incomingMessage);
		virtual ElectronicMessage* MessageCreated();
		virtual sc_time WaitTime(ElectronicMessage* message);
		
		Address addressSpace;
		Address nodeAddress;
		sc_time interarrivalTime;
		int messageSize;
		int numOfMessagesSent;
		int totalNumOfMessagesToSend;
		
		private:
		boost::variate_generator<boost::mt19937&, boost::exponential_distribution<> >* rng_interarrivaltime;
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> >* rng_nodeDestination;
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> >* rng_concDestination;
		
			static boost::mt19937 gen;
		
		bool waitForPreviousMessage;
		bool firstMessage;
		
		Address RandomDestination();
		
		ElectronicMessage *SendDataMessage();
		
	};
	
	// Register with TrafficGeneratorFactory
	namespace
	{
		TrafficGenerator* CreateRandomTrafficGenerator()
		{
			return new Random_TrafficGenerator;
		}
		const std::string identifier_tg_random = "random";
		
		const bool registered_tg_random = TrafficGeneratorFactory::Instance()->RegisterTrafficGenerator(CreateRandomTrafficGenerator, identifier_tg_random);
	}
}


#endif // __RANDOM_TRAFFIC_GENERATOR_H_