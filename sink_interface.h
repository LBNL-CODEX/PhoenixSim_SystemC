/*
 *  sink_interface.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/29/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __SINK_INTERFACE_H__
#define __SINK_INTERFACE_H__

#include <list>
#include <vector>

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc.h"
#include "source_interface.h"
#include "electronicmessage.h"

namespace PhoenixSim 
{
	class Sink_Interface : public Source_Interface
	{
	public:
		SC_HAS_PROCESS(Sink_Interface);
		
		Sink_Interface(sc_module_name name, int p_bufferWidth);
		~Sink_Interface();
		
	private:
		sc_spawn_options* spawnOptions;
		void NetworkMessageReceived(int inputPortId);
		void CreditReceived(int inputPortId);
	};
}

#endif // __SINK_INTERFACE_H__