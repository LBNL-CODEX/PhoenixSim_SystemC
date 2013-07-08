/*
 *  source_interface.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "source_interface.h"

#include <stdexcept>
#include <sstream>

#include "trafficgeneratorfactory.h"

using namespace std;
using namespace PhoenixSim;

Source_Interface::Source_Interface(sc_module_name name, int p_numOfNetworkPorts, int p_numOfProcessorPorts, int p_bufferWidth)
:Credit_Interface(name, p_numOfNetworkPorts, p_bufferWidth), numOfProcessorPorts(p_numOfProcessorPorts), processorDataInputPort(p_numOfProcessorPorts), processorDataOutputPort(p_numOfProcessorPorts)
{
	for(int i = 0; i < numOfProcessorPorts; i++)
	{
		std::stringstream so;
		so << "outputProcessorPort" << "<" << i << ">";
		processorDataOutputPort[i] = new sc_out<ElectronicMessage*>(so.str().c_str());
	}
	
	for(int i = 0; i < numOfProcessorPorts; i++)
	{		
		std::stringstream so;
		so << "inputProcessorPort" << "<" << i << ">";
		processorDataInputPort[i] = new sc_in<ElectronicMessage*>(so.str().c_str());
	}	
}

Source_Interface::~Source_Interface()
{
	for(int i = 0; i < numOfProcessorPorts; i++)
	{
		delete processorDataInputPort[i];
		delete processorDataOutputPort[i];
	}
}
