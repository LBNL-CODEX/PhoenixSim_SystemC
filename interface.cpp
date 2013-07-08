/*
 *  interface.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/24/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "interface.h"

#include <sstream>

using namespace PhoenixSim;

int Interface::currentMessageCount = 0;

Interface::Interface(sc_module_name name, int p_numOfNetworkPorts)
:sc_module(name), numOfNetworkPorts(p_numOfNetworkPorts), inputPort(p_numOfNetworkPorts), outputPort(p_numOfNetworkPorts)
{
	for(int i = 0; i < numOfNetworkPorts; i++)
	{
		std::stringstream so;
		so << "inputPort" << "<" << i << ">";
		inputPort[i] = new sc_in<ElectronicMessage*>(so.str().c_str());
	}
	
	for(int i = 0; i < numOfNetworkPorts; i++)
	{
		std::stringstream so;
		so << "outputPort" << "<" << i << ">";
		outputPort[i] = new sc_out<ElectronicMessage*>(so.str().c_str());
	}
}

Interface::~Interface()
{
	for(int i = 0; i < numOfNetworkPorts; i++)
	{
		delete inputPort[i];
		delete outputPort[i];
	}
}

int Interface::GetUniqueId()
{
	return currentMessageCount++;
}