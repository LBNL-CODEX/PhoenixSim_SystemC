/*
 *  routerarbiter.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/10/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ROUTERARBITER_H__
#define __ROUTERARBITER_H__


#define SC_INCLUDE_DYNAMIC_PROCESSES

#include <vector>
#include <list>
#include <string>

#include "systemc.h"
#include "electronicmessage.h"
#include "routingpolicy.h"

#include "orion/ORION_Arbiter.h"
#include "OrionPowerParams.h"

namespace PhoenixSim 
{
	SC_MODULE(RouterArbiter)
	{
		int numOfGrants; // number of grants to issue per clock cycle, maximum is the number of inputs, think of it as functional units
		std::vector<int> inputConnectedTo; // index labels input port, value indicates connected output (-1 for no connection) 
		std::vector<int> outputConnectedTo; // index labels output port, value indicates connected input (-1 for no connection) 
		std::vector<sc_time> inputEndTime; // index labels input port, time marks when packet finishes traversal
		std::vector<sc_time> outputEndTime; // index labels output port, time marks when packet finishes traversal
		std::vector< sc_out<bool>* > bufferPopOutputPort;
		std::vector< sc_out<int>* > crossbarControlOutputPort;
		std::vector<int> varCrossbarControlOutput;
		std::vector< sc_in<bool>* > bufferDataAvailablePort;
		std::vector< sc_in<ElectronicMessage*>* > dataInputPort;
		std::vector< sc_out<ElectronicMessage*>* > dataOutputPort;
		std::vector< sc_in<int>* > creditInputPort;
		std::vector< sc_out<int>* > creditOutputPort;
		
		
		std::vector<ElectronicMessage*> varDataOutput;
		bool needToWriteOutput;
		
		std::vector<int> remainingCredits;
		
		
		SC_HAS_PROCESS(RouterArbiter);
		
		RouterArbiter(sc_module_name name, std::string p_routingPolicy, RoutingParameters* p_routingParameters, int p_numOfInputPorts, int p_numOfOutputPorts, int p_numOfGrants, sc_time p_clockPeriod, int p_bufferDepth, int p_bufferWidth, const COrionPowerParams& powerParams, int p_numOfVirtualChannels = 1);
		~RouterArbiter();
		double GetCumulativePower();	
		
		private:
		int numOfInputPorts;
		int numOfOutputPorts;
		sc_time clockPeriod;
		int bufferDepth;
		int bufferWidth;
		int numOfVirtualChannels;
		RoutingPolicy* routingPolicy;
		int routerId;
		sc_event updateEvent, popEvent;
		
		
		void RoutePacket();
		void SetUpdateDelay();
		
		
		std::vector<sc_spawn_options*> spawnOptions;
		void receiveCredit(int portId);
		
		std::list<int>* selectBuffer();
		int nextBufferToCheck;

		std::map<int, ORION_Arbiter*> power_;

	};
}

#endif // __ROUTERARBITER_H__

