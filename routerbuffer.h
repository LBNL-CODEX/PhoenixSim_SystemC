/*
 *  routerbuffer.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/9/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ROUTERBUFFER_H__
#define __ROUTERBUFFER_H__

#include <list>

#include "systemc.h"
#include "electronicmessage.h"

#include "orion/ORION_Array.h"
#include "OrionPowerParams.h"

namespace PhoenixSim 
{
	SC_MODULE(RouterBuffer)
	{
		sc_in<bool>* pop;
		sc_out<bool>* ready;

		sc_in<ElectronicMessage*>* inputPort;
		sc_out<ElectronicMessage*>* outputPort;
		
		SC_HAS_PROCESS(RouterBuffer);
		
		RouterBuffer(sc_module_name name, sc_time p_clockPeriod, int p_bufferDepth, int p_bufferWidth, const COrionPowerParams& powerParams);
		~RouterBuffer();
		double	GetCumulativePower(){return energy_;};
		
		private:
		std::list<ElectronicMessage*> queuedItems;
		
		sc_time clockPeriod;
		int bufferDepth;
		int bufferWidth;
		int creditsRemaining;
		
		sc_event dataReady;
		
		void PushOntoQueue();
		void ModifyOutput();
		void PopOffQueue();

		ORION_Array* power_;
		double energy_;
	};
}

#endif // __ROUTERBUFFER_H__

