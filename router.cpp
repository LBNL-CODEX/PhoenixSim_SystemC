/*
 *  router.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/10/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "router.h"

#include <sstream>

using namespace std;
using namespace PhoenixSim;

Router::Router(sc_module_name name, string p_routingPolicy, RoutingParameters* p_routingParameters, unsigned int p_numOfInputPorts, unsigned int p_numOfOutputPorts, unsigned int p_numOfGrants, sc_time p_clockPeriod, unsigned int p_bufferDepth, unsigned int p_bufferWidth, const COrionPowerParams& powerParams, unsigned int p_numOfVirtualChannels)
: sc_module(name), numOfInputPorts(p_numOfInputPorts), numOfOutputPorts(p_numOfOutputPorts), clockPeriod(p_clockPeriod), bufferDepth(p_bufferDepth), bufferWidth(p_bufferWidth), numOfVirtualChannels(p_numOfVirtualChannels), routerName(name)
{
	// Instantiate modules
	
	crossbar = new RouterCrossbar("crossbar", numOfInputPorts, numOfOutputPorts, clockPeriod, powerParams);
	arbiter = new RouterArbiter("arbiter", p_routingPolicy, p_routingParameters, numOfInputPorts, numOfOutputPorts, p_numOfGrants, clockPeriod, bufferDepth, bufferWidth, powerParams, numOfVirtualChannels);
	buffer.resize(numOfInputPorts * numOfVirtualChannels);
	for (int i = 0; i < numOfInputPorts; i++) 
	{
		for( int j = 0; j < numOfVirtualChannels; j++)
		{
			std::stringstream so;
			so << "buffer" << "<" << i << "," << j << ">";
			buffer[i * numOfVirtualChannels + j] = new RouterBuffer(so.str().c_str(), clockPeriod, bufferDepth, bufferWidth, powerParams);
		}
	}	
	 
	 
	// Instantiate signals and ports
	
	inputPort.resize(numOfInputPorts);
	outputPort.resize(numOfOutputPorts);
	
	creditInputPort.resize(numOfOutputPorts);
	creditOutputPort.resize(numOfInputPorts);
	
	sig_bufferPop.resize(numOfInputPorts);
	sig_dataAvailable.resize(numOfInputPorts);
	sig_headerData.resize(numOfInputPorts);
	
	buf_dataArbiterToSwitch.resize(numOfInputPorts);
	sig_controlArbiterToSwitch.resize(numOfInputPorts);
	
	virtualchannel_spawnoptions.resize(numOfInputPorts);
	
	for(int i = 0; i < numOfInputPorts; i++)
	{
		
	}
	
	
	for (int i = 0; i < numOfInputPorts; i++) 
	{
		std::stringstream so, so1, so2, so3, so4, so5, so6;
		so1 << "buf_arbToSwitch" << "<" << i << ">";
		buf_dataArbiterToSwitch[i] = new sc_signal<ElectronicMessage*>(so1.str().c_str());
		so2 << "port_inputPort" << "<" << i << ">";
		inputPort[i] = new sc_in<ElectronicMessage*>(so2.str().c_str());
		so4 << "buf_bufferPop" << "<" << i << ">";
		sig_bufferPop[i] = new sc_buffer<bool>(so4.str().c_str());
		so3 << "sig_dataAvailable"<< "<" << i << ">";
		sig_dataAvailable[i] = new sc_buffer<bool>(so3.str().c_str());
		so << "sig_headerData"<< "<" << i << ">";
		sig_headerData[i] = new sc_signal<ElectronicMessage*>(so.str().c_str());
		so5 << "sig_arbiterToSwitch" << "<" << i << ">";
		sig_controlArbiterToSwitch[i] = new sc_signal<int>(so5.str().c_str());
		so6 << "port_creditOutputPort" << "<" << i << ">";
		creditOutputPort[i] = new sc_out<int>(so6.str().c_str());
		
	}
	
	for (int i = 0; i < numOfOutputPorts; i++) 
	{
		std::stringstream so ,so1;
		so << "port_outputPort" << "<" << i << ">";
		outputPort[i] = new sc_out<ElectronicMessage*>(so.str().c_str());
		so1 << "port_creditInputPort" << "<" << i << ">";
		creditInputPort[i] = new sc_in<int>(so1.str().c_str());
	}
	
	// Connect modules and ports
	
	
	//(*(crossbar->clock))(*clock);
	
	
	
	for (int i = 0; i < numOfInputPorts; i++) 
	{
		// Inputs to Buffers
		buffer[i]->inputPort->bind(*inputPort[i]);
		
		// Credit Ports from Arbiter
		arbiter->creditOutputPort[i]->bind(*creditOutputPort[i]);
		
		// Arbiter to Crossbar
		arbiter->dataOutputPort[i]->bind(*buf_dataArbiterToSwitch[i]);
		crossbar->dataInputPort[i]->bind(*buf_dataArbiterToSwitch[i]);
		
		// Pop, Header, and Available Signals between Buffers and Arbiter
		buffer[i]->pop->bind(*sig_bufferPop[i]);
		arbiter->bufferPopOutputPort[i]->bind(*sig_bufferPop[i]);
		
		buffer[i]->ready->bind(*sig_dataAvailable[i]);
		arbiter->bufferDataAvailablePort[i]->bind(*sig_dataAvailable[i]);
		
		buffer[i]->outputPort->bind(*sig_headerData[i]);
		arbiter->dataInputPort[i]->bind(*sig_headerData[i]);
		
		// Arbiter to Crossbar
		crossbar->controlInputPort[i]->bind(*sig_controlArbiterToSwitch[i]);
		arbiter->crossbarControlOutputPort[i]->bind(*sig_controlArbiterToSwitch[i]);
		
	}
	
	for (int i = 0; i < numOfOutputPorts; i++)
	{
		// Crossbar to Outputs
		crossbar->dataOutputPort[i]->bind(*outputPort[i]);
		
		// Credit Ports to Arbiter
		arbiter->creditInputPort[i]->bind(*creditInputPort[i]);
	}
}

Router::~Router()
{
	delete crossbar;
	delete arbiter;
	
	for (int i = 0; i < numOfInputPorts; i++) 
	{
		delete buffer[i];
		delete inputPort[i];
		delete buf_dataArbiterToSwitch[i];
		delete sig_bufferPop[i];
		delete sig_dataAvailable[i];
		delete sig_headerData[i];
		delete sig_controlArbiterToSwitch[i];
		delete creditOutputPort[i];
	}
	
	for (int i = 0; i < numOfOutputPorts; i++) 
	{
		delete outputPort[i];
		delete creditInputPort[i];
	}
}


double Router::GetCumulativePower(){
	
	double totalEnergy = crossbar->GetCumulativePower();
	totalEnergy +=       arbiter->GetCumulativePower();
	for (int i = 0; i < numOfInputPorts; i++){
		totalEnergy += buffer[i]->GetCumulativePower();
	}
	return totalEnergy;
}

void Router::sc_trace (sc_trace_file *tf, const std::string &NAME)
{
	for(int i = 0; i < numOfInputPorts; i++)
	{
		std::stringstream so;
		so << NAME <<".inputPort" << "<" << i << ">";
		//sc_core::sc_trace(tf, inputPort[i]->read().messageId, so.str());
		sc_core::sc_trace(tf, sig_headerData[i]->read()->messageId, so.str());
	}
	
	for(int i = 0; i < numOfOutputPorts; i++)
	{		
		std::stringstream so;
		so << "routputPort" << "<" << i << ">";
		//sc_core::sc_trace(tf, outputPort[i]->read().messageId, so.str());
	}
}
