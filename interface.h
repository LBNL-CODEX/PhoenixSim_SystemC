/*
 *  interface.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/24/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *	Abstract class for interface between data generation and network. Only
 *	offers variable length input and output ports and unique message ID numbers.
 *	All other interfaces should be derived from Interface.
 */

#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include <list>
#include <vector>

#include "systemc.h"
#include "electronicmessage.h"

namespace PhoenixSim 
{
	class Interface : public sc_module
	{
	public:
		Interface(sc_module_name name, int p_numofNetworkPorts);
		~Interface();
		
		std::vector< sc_in<ElectronicMessage*>* > inputPort;
		std::vector< sc_out<ElectronicMessage*>* > outputPort;
		
	protected:
		const int numOfNetworkPorts;
		
		virtual void NetworkMessageReceived(int inputPortId) = 0;
		
		static int GetUniqueId();
	private:
		static int currentMessageCount;
		
	};
}

#endif // __INTERFACE_H__

