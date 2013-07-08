/*
 *  router_crossbar.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/3/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ROUTERCROSSBAR_H__
#define __ROUTERCROSSBAR_H__

#include <vector>

#include "systemc.h"
#include "electronicmessage.h"

#include "orion/ORION_Crossbar.h"
#include "OrionPowerParams.h"

namespace PhoenixSim 
{
	SC_MODULE(RouterCrossbar)
	{
		std::vector< sc_in<int>* > controlInputPort;
		std::vector< sc_in<ElectronicMessage*>* > dataInputPort;
		std::vector< sc_out<ElectronicMessage*>* > dataOutputPort;
		
		SC_HAS_PROCESS(RouterCrossbar);
		
		RouterCrossbar(sc_module_name name, int p_numOfInputPorts, int p_numOfOutputPorts, sc_time p_clockPeriod, const COrionPowerParams& powerParams);
		~RouterCrossbar();
		double	GetCumulativePower(){return (power_->report() + power_->report_static_power());};
		double  GetTotalMessagesRouted(){return messagesRouted;};
		
		private:
		int numOfInputPorts;
		int numOfOutputPorts;
		sc_time clockPeriod;
		
		sc_event_queue updateEvent;
		
		void SetUpdateDelay();
		void UpdateCrossbar();
		double messagesRouted;

		ORION_Crossbar* power_;
	};
}

#endif // __ROUTERCROSSBAR_H__

