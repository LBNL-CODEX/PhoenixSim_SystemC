/*
 *  electronicpacket_interface.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ELECTRONICPACKET_INTERFACE_H__
#define __ELECTRONICPACKET_INTERFACE_H__

#include <string>
#include <queue>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc.h"

#include "source_interface.h"
#include "trafficgenerator.h"
#include "address.h"


const int DATA_HEADER_SIZE_ = 0;

namespace PhoenixSim 
{
	class ElectronicPacket_Interface : public Source_Interface
	{
	public:
		SC_HAS_PROCESS(ElectronicPacket_Interface);
		
		ElectronicPacket_Interface(sc_module_name name, sc_time p_clockPeriod, int p_bufferWidth, int p_bufferDepth, int p_maxPacketSize);
		~ElectronicPacket_Interface();
		
		
		void ProcessorDataReceived(int inputPortId);
		void NetworkMessageReceived(int inputPortId);
		
		void CreditReceived(int inputPortId);
		
	private:
		
		int numOfProcessorPorts;
		
		sc_time clockPeriod;
		int bufferWidth;
		
		std::queue<ElectronicMessage*> messageQueue;
		
		
		sc_spawn_options* creditReceivedOptions;
		sc_spawn_options* networkMessageReceivedOptions;
		sc_spawn_options* processorDataReceivedOptions;
		
		int creditsRemaining;
		int bufferDepth;
		int maxPacketSize;
		
		sc_event controllerEvent;
		void Controller();
		
		ElectronicMessage* currentMessage;
		ElectronicMessage* currentPacket;
		int currentSequenceId;
		int remainingData;
		
		bool requestSuccessful;
		bool transmissionComplete;
		
		ElectronicMessage* receivingMessage;
		
		
		bool Request();
		bool Prepare();
		sc_time Send();
		sc_time channelReadyTime;
		void Complete();
	};
}

#endif // __ELECTRONICPACKET_INTERFACE_H__



