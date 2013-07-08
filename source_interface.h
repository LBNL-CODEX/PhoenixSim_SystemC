/*
 *  source_interface.h
 *  PhoenixSim
 *
 *  Created by ; Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __SOURCE_INTERFACE_H__
#define __SOURCE_INTERFACE_H__

#include <string>
#include <queue>

#include "systemc.h"

#include "credit_interface.h"
#include "trafficgenerator.h"
#include "address.h"

namespace PhoenixSim 
{
	class Source_Interface : public Credit_Interface
	{
	public:
		Source_Interface(sc_module_name name, int p_numOfNetworkPorts, int p_numOfProcessorPorts, int p_bufferWidth);
		~Source_Interface();
		
		std::vector< sc_in<ElectronicMessage*>* > processorDataInputPort;
		std::vector< sc_out<ElectronicMessage*>* > processorDataOutputPort;
	
		virtual void ProcessorDataReceived(int inputPortId) = 0;
	private:
		unsigned int numOfProcessorPorts;
		
	};
}

#endif // __SOURCE_INTERFACE_H__

