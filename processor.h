/*
 *  processor.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/11/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __PROCESSOR_H__
#define __PROCESSOR_H__

#include <queue>
#include <string>

#include "systemc.h"
#include "electronicmessage.h"
#include "trafficgenerator.h"
#include "average_statisticsmachine.h"
#include "single_statisticsmachine.h"

namespace PhoenixSim 
{
	class Processor : public sc_module
	{
	public:
		SC_HAS_PROCESS(Processor);
		
		Processor(sc_module_name name, std::string p_generator, TrafficGeneratorParameters* p_parameters);
		~Processor();
		
		sc_in<ElectronicMessage*>* inputPort;
		sc_out<ElectronicMessage*>* outputPort;
		
	protected:

	private:
		static Average_StatisticsMachine networkLatency;
		static Single_StatisticsMachine lastMessageTime;
		static bool once;
		static unsigned int currentMessageCount;
		
		TrafficGenerator* trafficGenerator;
		
		
		sc_event_queue sendMessageEvent;
		
		void Initialize();
		
		void MessageReceived();
		
		void MessageTransmitted();
		
		void SendMessage();
		std::queue<ElectronicMessage*> messageQueue;
		
	};
}

#endif // __PROCESSOR_H__

