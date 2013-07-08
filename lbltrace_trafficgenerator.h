/*
 *  lbltrace_trafficgenerator.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/15/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *	This parser handles traces obtain from LBL (circa 2009). The format of the 
 *	trace files is <phase>,<source id>,<destination id>,<message size>. All 
 *	messages appear in this file in chronological order, and should appear one
 *	per line. Also last line should (probably) be returned.
 *
 *	<phase>	- specifies the phase of the message. Is monotonically increasing as
 *				you read down the file.
 *	<source/destination id> - specifies the source and destination kernel of the
 *				message. This is distinct from the source and destination
 *				processor, which are assigned randomly during initialization.
 *	<message size> - specifies the message size, in bytes. Must be converted to
 *				bits when stored into the PS++ data structure.
 *	
 *
 *	The default protocol is to transmit messages as fast as possible for the 
 *	current phase from the message queue. If no messages for the current phase 
 *	remain and the processor does not expect anymore received messages, 
 *  then an "ack" message is transmitted to the master processor (which is 
 *	assigned kernel #0). When the master processor receives an "ack" from every 
 *	other kernel, then the master processor broadcasts another "ack" which 
 *	indicates all processors to move onto the next phase.
 */

#ifndef __LBLTRACE_TRAFFICGENERATOR_H__
#define __LBLTRACE_TRAFFICGENERATOR_H__

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
	class LBLTrace_TrafficGeneratorParameters: public TrafficGeneratorParameters
	{
	public:
		LBLTrace_TrafficGeneratorParameters();
		
		Address addressSpace;
		Address nodeAddress;
		int networkSize;
		std::string fileName;
	};
	
	class LBLTrace_TrafficGenerator: public TrafficGenerator
	{
	public:
		
		LBLTrace_TrafficGenerator();
		~LBLTrace_TrafficGenerator();
		
		void SetTrafficGeneratorParameters(TrafficGeneratorParameters *p_parameters);
		
		virtual ElectronicMessage* CreateFirstMessage();
		virtual ElectronicMessage* MessageReceived(ElectronicMessage* incomingMessage);
		virtual ElectronicMessage* MessageCreated();
		
		Address addressSpace;
		Address nodeAddress;
		int networkSize;
	
	private:
		
		void InitializeKernelInterfaceMapping();
		void RandomKernelInterfaceMapping();
		
		// index is the processor address, value is the kernel id
		static std::map<Address, int> kernelMap;
		
		// index is the kernel id, value is the processor id
		static std::map<int, Address> processorMap;
		static bool isInitialized;
		
		static boost::mt19937 gen;
		
		std::ifstream traceFile;
		
		int currentPhase;
		int lastPhase;
		
		std::queue<int> messagePhaseQueue;
		std::queue<int> messageDestinationKernelQueue;
		std::queue<int> messageSizeQueue;
		
		std::map<int, int> expectedMessages; // key = phase #, value = expected # of messages
				
		ElectronicMessage* SendDataMessage(int destinationKernelId, int messageSize);
		
		ElectronicMessage* SendAckMessage(int kernelId);
		
		void MasterKernelReceivedAck();
		
		
		int ackCount;
		bool waitForAck;
		bool startNextPhase;
		
		bool traceDone;
	
	};
	
	// Register with TrafficGeneratorFactory
	namespace
	{
		TrafficGenerator* CreateLBLTraceTrafficGenerator()
		{
			return new LBLTrace_TrafficGenerator;
		}
		const std::string identifier_lbl_trace = "lbltrace";
		
		const bool registered_lbl_trace = TrafficGeneratorFactory::Instance()->RegisterTrafficGenerator(CreateLBLTraceTrafficGenerator, identifier_lbl_trace);
	}
}


#endif // __LBLTRACE_TRAFFICGENERATOR_H__