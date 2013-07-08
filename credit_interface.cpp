/*
 *  credit_interface.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/28/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "credit_interface.h"

#include <sstream>

using namespace PhoenixSim;

Credit_Interface::Credit_Interface(sc_module_name name, int p_numOfNetworkPorts, int p_bufferWidth)
:Interface(name, p_numOfNetworkPorts), creditInputPort(p_numOfNetworkPorts), creditOutputPort(p_numOfNetworkPorts), bufferWidth(p_bufferWidth)
{
	for(int i = 0; i < numOfNetworkPorts; i++)
	{
		std::stringstream so;
		so << "outputCreditPort" << "<" << i << ">";
		creditOutputPort[i] = new sc_out<int>(so.str().c_str());
	}
	
	for(int i = 0; i < numOfNetworkPorts; i++)
	{		
		std::stringstream so;
		so << "inputCreditPort" << "<" << i << ">";
		creditInputPort[i] = new sc_in<int>(so.str().c_str());
	}
}

Credit_Interface::~Credit_Interface()
{
	for(int i = 0; i < numOfNetworkPorts; i++)
	{
		delete creditOutputPort[i];
	}
	
	for(int i = 0; i < numOfNetworkPorts; i++)
	{
		delete creditInputPort[i];
	}
}