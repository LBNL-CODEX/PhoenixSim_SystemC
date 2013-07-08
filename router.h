/*
 *  router.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/10/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ROUTER_H__
#define __ROUTER_H__

#include <string>

#include "systemc.h"
#include "electronicmessage.h"
#include "routercrossbar.h"
#include "routerbuffer.h"
#include "routerarbiter.h"
#include "OrionPowerParams.h"

namespace PhoenixSim 
{	
	
	SC_MODULE(Router)
	{
		// Internal Variables
		int numOfInputPorts;
		int numOfOutputPorts;
		sc_time clockPeriod;
		int bufferDepth;
		int bufferWidth;
		int numOfVirtualChannels;
		
		// Modules
		RouterCrossbar* crossbar;
		std::vector<RouterBuffer*> buffer;
		RouterArbiter* arbiter;
		
		// Ports
		std::vector< sc_in<ElectronicMessage*>* > inputPort;
		std::vector< sc_out<ElectronicMessage*>* > outputPort;
		
		std::vector< sc_in<int>* > creditInputPort;
		std::vector< sc_out<int>* > creditOutputPort;
	
		double	GetCumulativePower();	
		double  GetTotalMessagesRouted(){return crossbar->GetTotalMessagesRouted();}	
		sc_core::sc_module_name	routerName;		

		// Internal Signals
		
		std::vector< sc_buffer<bool>* > sig_bufferPop;
		std::vector< sc_buffer<bool>* > sig_dataAvailable;
		std::vector< sc_signal<ElectronicMessage*>* > sig_headerData;
		std::vector< sc_signal<ElectronicMessage*>* > buf_dataArbiterToSwitch;
		std::vector< sc_signal<int>* > sig_controlArbiterToSwitch;
		
		SC_HAS_PROCESS(Router);
		
		Router(sc_module_name name, std::string p_routingPolicy, RoutingParameters* p_routingParameters, unsigned int p_numOfInputPorts, unsigned int p_numOfOutputPorts, unsigned int p_numOfGrants, sc_time p_clockPeriod, unsigned int p_bufferDepth, unsigned int p_bufferWidth, const COrionPowerParams& powerParams, unsigned int p_numOfVirtualChannels = 1);
		
		~Router();
		
		std::vector<sc_spawn_options*> virtualchannel_spawnoptions;
		void VirtualChannelSelect(int inputPort);
		
		void sc_trace (sc_trace_file *tf, const std::string &NAME);
		
	};
}

#endif // __ROUTER_H__

