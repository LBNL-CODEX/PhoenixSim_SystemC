/*
 *  trafficgenerator.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __TRAFFICGENERATOR_H__
#define __TRAFFICGENERATOR_H__

#include "systemc.h"

#include "electronicmessage.h"

namespace PhoenixSim
{
	class TrafficGeneratorParameters
	{
		
	};
	
	class TrafficGenerator
	{
		public:
			TrafficGenerator();
			virtual ~TrafficGenerator();
		
			virtual void SetTrafficGeneratorParameters(TrafficGeneratorParameters *p_parameters) = 0;
			
			virtual ElectronicMessage* CreateFirstMessage();
			virtual ElectronicMessage* MessageReceived(ElectronicMessage* incomingMessage);
			virtual ElectronicMessage* MessageTransmitted();
			virtual ElectronicMessage* MessageCreated();
			virtual sc_time WaitTime(ElectronicMessage* message);

			static int GetUniqueId();
		private:
			static int currentMessageCount;
	};
}

#endif // __TRAFFICGENERATOR_H__

