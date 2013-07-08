/*
 *  lbltrace_ackall_trafficgenerator.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/19/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __LBLTRACE_ACKALL_TRAFFICGENERATOR_H__
#define __LBLTRACE_ACKALL_TRAFFICGENERATOR_H__

#include <string>
#include <map>
#include <queue>

#include "systemc.h"

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/variate_generator.hpp>

#include "trafficgenerator.h"
#include "trafficgeneratorfactory.h"
#include "electronicmessage.h"
#include "address.h"

namespace PhoenixSim
{
	class LBLTrace_AckAll_TrafficGeneratorParameters: public TrafficGeneratorParameters
	{
	public:
		LBLTrace_AckAll_TrafficGeneratorParameters();
		
		Address addressSpace;
		Address nodeAddress;
		int networkSize;
		std::string fileName;
	};
	
	class LBLTrace_AckAll_TrafficGenerator: public TrafficGenerator
	{
	public:
		
		LBLTrace_AckAll_TrafficGenerator();
		~LBLTrace_AckAll_TrafficGenerator();
		
		void SetTrafficGeneratorParameters(TrafficGeneratorParameters *p_parameters);
		
		virtual ElectronicMessage* CreateFirstMessage();
		virtual ElectronicMessage* MessageReceived(ElectronicMessage* incomingMessage);
		virtual ElectronicMessage* MessageCreated();
		
		Address addressSpace;
		Address nodeAddress;
		int nodeKernelId;
		int networkSize;
		std::string fileName;
		
	private:
		
		void InitializeDataStructures();
		void RandomKernelInterfaceMapping();
		
		// index is the processor address, value is the kernel id
		static std::map<Address, int> kernelMap;
		
		// index is the kernel id, value is the processor id
		static std::map<int, Address> processorMap;
		static bool isInitialized;
		
		static boost::mt19937 gen;
		
		std::ifstream traceFile;
		
		int currentPhase;
		static int lastPhase;
		
		static std::map<int, std::queue<int> > messagePhaseQueue;
		static std::map<int, std::queue<int> > messageDestinationKernelQueue;
		static std::map<int, std::queue<int> > messageSizeQueue;
		
		ElectronicMessage* SendDataMessage(int destinationKernelId, int messageSize);
		
		ElectronicMessage* SendAckMessage(int kernelId);
		ElectronicMessage* SendMessageAckMessage(Address destinationAddress);
		
		void MasterKernelReceivedAck();
		
		bool waitingForMessageAck;
		
		int ackCount;
		bool waitForAck;
		bool startNextPhase;
		
		bool traceDone;
		
	};
	
	// Register with TrafficGeneratorFactory
	namespace
	{
		TrafficGenerator* CreateLBLTraceAckAllTrafficGenerator()
		{
			return new LBLTrace_AckAll_TrafficGenerator;
		}
		const std::string identifier_lbl_trace_ackall = "lbltrace_ackall";
		
		const bool registered_lbl_trace_ackall = TrafficGeneratorFactory::Instance()->RegisterTrafficGenerator(CreateLBLTraceAckAllTrafficGenerator, identifier_lbl_trace_ackall);
	}
}


#endif // __LBLTRACE_ACKALL_TRAFFICGENERATOR_H__