/*
 *  electronicfoldedtorus_topology.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/5/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "electronictorus_topology.h"

#include <stdexcept>
#include <sstream>

#include "electronicmesh_xy_routingpolicy.h"
#include "electronictorus_xy_routingpolicy.h"
#include "electronicfoldedtorus_xy_routingpolicy.h"

using namespace std;
using namespace PhoenixSim;

ElectronicTorus_Topology::ElectronicTorus_Topology(sc_module_name name, const ElectronicTorus_Parameters & parameters, const COrionPowerParams& powerParams)
: sc_module(name), networkSizeX(parameters.networkSizeX), networkSizeY(parameters.networkSizeY), concentrationType(parameters.concentrationType), concentrationLevel(parameters.concentrationLevel)
{
	int nodePortsRequired;
	
	
	
	
	if(concentrationType == CONC_NONE)
	{
		nodePortsRequired = 5;
		assert(concentrationLevel == 1);
	}
	else if(concentrationType == CONC_NODE)
	{
		nodePortsRequired = 4 + concentrationLevel;
	}
	else 
	{
		throw logic_error("in 'electronictorus_topology.cpp' - Invalid concentration type.");
	}

	
	// Instantiate Modules
	nodes.resize(networkSizeX * networkSizeY * concentrationLevel);
	
	ElectronicFoldedTorus_XY_RoutingParameters* parameters_ft;
	ElectronicTorus_XY_RoutingParameters* parameters_t;
	ElectronicMesh_XY_RoutingParameters* parameters_m;
	
	// Select between folded torus and unfolded torus topology. Difference is in wirelengths
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		
		if(parameters.torusType == ELEC_FOLDEDTORUS)
		{
			parameters_ft = new ElectronicFoldedTorus_XY_RoutingParameters;
			parameters_ft->networkSizeX = networkSizeX;
			parameters_ft->networkSizeY = networkSizeY;
			parameters_ft->bufferWidth = parameters.bufferWidth;
			parameters_ft->numOfVirtualChannels = parameters.numOfVirtualChannels;
			parameters_ft->maxPacketSizeInCredits = ceil(parameters.maxPacketSize / double(parameters.bufferWidth));
			std::stringstream so;
			so << "router" << "<" << i << ">";
			parameters_ft->routerId = i;
			nodes[i] = new Router(so.str().c_str(), "electronicfoldedtorus_xy", parameters_ft, nodePortsRequired, nodePortsRequired, parameters.numOfGrants, parameters.clockPeriod, parameters.bufferDepth, parameters.bufferWidth, powerParams);
			delete parameters_ft; 
		}
		else if(parameters.torusType == ELEC_TORUS)
		{
			parameters_t = new ElectronicTorus_XY_RoutingParameters;
			parameters_t->networkSizeX = networkSizeX;
			parameters_t->networkSizeY = networkSizeY;
			parameters_t->bufferWidth = parameters.bufferWidth;
			parameters_t->numOfVirtualChannels = parameters.numOfVirtualChannels;
			parameters_t->maxPacketSizeInCredits = ceil(parameters.maxPacketSize / double(parameters.bufferWidth));
			std::stringstream so;
			so << "router" << "<" << i << ">";
			parameters_t->routerId = i;
			nodes[i] = new Router(so.str().c_str(), "electronictorus_xy", parameters_t, nodePortsRequired, nodePortsRequired, parameters.numOfGrants, parameters.clockPeriod, parameters.bufferDepth, parameters.bufferWidth, powerParams);
			delete parameters_t; 
		}
		else if(parameters.torusType == ELEC_MESH)
		{
			parameters_m = new ElectronicMesh_XY_RoutingParameters;
			parameters_m->networkSizeX = networkSizeX;
			parameters_m->networkSizeY = networkSizeY;
			std::stringstream so;
			so << "router" << "<" << i << ">";
			parameters_m->routerId = i;
			nodes[i] = new Router(so.str().c_str(), "electronicmesh_xy", parameters_m, nodePortsRequired, nodePortsRequired, parameters.numOfGrants, parameters.clockPeriod, parameters.bufferDepth, parameters.bufferWidth, powerParams);
			delete parameters_m; 
		}
		else 
		{
			std::cout<<"1"<<endl;
			throw logic_error("in 'electronictorus_topology.cpp' - Unknown network type.");
		}


	}
	
	
	// Instantiate Ports and Channels
	
	inputPort.resize(networkSizeX * networkSizeY * concentrationLevel);
	outputPort.resize(networkSizeX * networkSizeY * concentrationLevel);
	
	inputCreditPort.resize(networkSizeX * networkSizeY * concentrationLevel);
	outputCreditPort.resize(networkSizeX * networkSizeY * concentrationLevel);
	
	for(int i = 0; i < networkSizeX * networkSizeY * concentrationLevel; i++)
	{
		std::stringstream so, so1, so2, so3;
		so << "port_input" << "<" << i << ">";
		inputPort[i] = new sc_in<ElectronicMessage*>(so.str().c_str());
		so1 << "port_output" << "<" << i << ">";
		outputPort[i] = new sc_out<ElectronicMessage*>(so1.str().c_str());
		so2 << "port_inputcredit" << "<" << i << ">";
		inputCreditPort[i] = new sc_in<int>(so2.str().c_str());
		so3 << "port_outputcredit" << "<" << i << ">";
		outputCreditPort[i] = new sc_out<int>(so3.str().c_str());
	}	
	
	horizontalLeftPathA.resize(networkSizeX * networkSizeY);
	horizontalRightPathA.resize(networkSizeX * networkSizeY);
	verticalUpPathA.resize(networkSizeX * networkSizeY);
	verticalDownPathA.resize(networkSizeX * networkSizeY);
	
	horizontalLeftCreditPathA.resize(networkSizeX * networkSizeY);
	horizontalRightCreditPathA.resize(networkSizeX * networkSizeY);
	verticalUpCreditPathA.resize(networkSizeX * networkSizeY);
	verticalDownCreditPathA.resize(networkSizeX * networkSizeY);
	
	horizontalLeftPathB.resize(networkSizeX * networkSizeY);
	horizontalRightPathB.resize(networkSizeX * networkSizeY);
	verticalUpPathB.resize(networkSizeX * networkSizeY);
	verticalDownPathB.resize(networkSizeX * networkSizeY);
	
	horizontalLeftCreditPathB.resize(networkSizeX * networkSizeY);
	horizontalRightCreditPathB.resize(networkSizeX * networkSizeY);
	verticalUpCreditPathB.resize(networkSizeX * networkSizeY);
	verticalDownCreditPathB.resize(networkSizeX * networkSizeY);
	
	
	messageWiresHorizontalLeft.resize(networkSizeX * networkSizeY);
	messageWiresHorizontalRight.resize(networkSizeX * networkSizeY);
	
	messageWiresVerticalUp.resize(networkSizeX * networkSizeY);
	messageWiresVerticalDown.resize(networkSizeX * networkSizeY);
	
	creditWiresHorizontalLeft.resize(networkSizeX * networkSizeY);
	creditWiresHorizontalRight.resize(networkSizeX * networkSizeY);
	
	creditWiresVerticalUp.resize(networkSizeX * networkSizeY);
	creditWiresVerticalDown.resize(networkSizeX * networkSizeY);
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		std::stringstream so, so1, so2, so3;
		so << "buf_horizontalLeftPathA" << "<" << i << ">";
		horizontalLeftPathA[i] = new sc_buffer<ElectronicMessage*>(so.str().c_str());
		so1 << "buf_horizontalRightPathA" << "<" << i << ">";
		horizontalRightPathA[i] = new sc_buffer<ElectronicMessage*>(so1.str().c_str());
		so2 << "buf_horizontalLeftCreditPathA" << "<" << i << ">";
		horizontalLeftCreditPathA[i] = new sc_buffer<int>(so2.str().c_str());
		so3 << "buf_horizontalRightCreditPathA" << "<" << i << ">";
		horizontalRightCreditPathA[i] = new sc_buffer<int>(so3.str().c_str());
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		std::stringstream so, so1, so2, so3;
		so << "buf_horizontalLeftPathB" << "<" << i << ">";
		horizontalLeftPathB[i] = new sc_buffer<ElectronicMessage*>(so.str().c_str());
		so1 << "buf_horizontalRightPathB" << "<" << i << ">";
		horizontalRightPathB[i] = new sc_buffer<ElectronicMessage*>(so1.str().c_str());
		so2 << "buf_horizontalLeftCreditPathB" << "<" << i << ">";
		horizontalLeftCreditPathB[i] = new sc_buffer<int>(so2.str().c_str());
		so3 << "buf_horizontalRightCreditPathB" << "<" << i << ">";
		horizontalRightCreditPathB[i] = new sc_buffer<int>(so3.str().c_str());
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{	
		std::stringstream so, so1, so2, so3;
		so << "buf_verticalUpPathA" << "<" << i << ">";
		verticalUpPathA[i] = new sc_buffer<ElectronicMessage*>(so.str().c_str());
		so1 << "buf_verticalDownPathA" << "<" << i << ">";
		verticalDownPathA[i] = new sc_buffer<ElectronicMessage*>(so1.str().c_str());
		so2 << "buf_verticalUpCreditPathA" << "<" << i << ">";
		verticalUpCreditPathA[i] = new sc_buffer<int>(so2.str().c_str());
		so3 << "buf_verticalDownCreditPathA" << "<" << i << ">";
		verticalDownCreditPathA[i] = new sc_buffer<int>(so3.str().c_str());
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{	
		std::stringstream so, so1, so2, so3;
		so << "buf_verticalUpPathB" << "<" << i << ">";
		verticalUpPathB[i] = new sc_buffer<ElectronicMessage*>(so.str().c_str());
		so1 << "buf_verticalDownPathB" << "<" << i << ">";
		verticalDownPathB[i] = new sc_buffer<ElectronicMessage*>(so1.str().c_str());
		so2 << "buf_verticalUpCreditPathB" << "<" << i << ">";
		verticalUpCreditPathB[i] = new sc_buffer<int>(so2.str().c_str());
		so3 << "buf_verticalDownCreditPathB" << "<" << i << ">";
		verticalDownCreditPathB[i] = new sc_buffer<int>(so3.str().c_str());
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{	
		std::stringstream so, so1, so2, so3;
		so << "messagewire_horizontalleft" << "<" << i << ">";
		so1 << "messagewire_horizontalright" << "<" << i << ">";
		so2 << "messagewire_verticalup" << "<" << i << ">";
		so3 << "messagewire_verticaldown" << "<" << i << ">";
		
		if(parameters.torusType == ELEC_TORUS && i % networkSizeX == 0)
		{
			messageWiresHorizontalLeft[i] = new Wire<ElectronicMessage*>(so.str().c_str(), sc_time(1, SC_NS), parameters.longPathChannelStages, powerParams);
			messageWiresHorizontalRight[i] = new Wire<ElectronicMessage*>(so1.str().c_str(), sc_time(1, SC_NS), parameters.longPathChannelStages, powerParams);
		}
		else
		{
			messageWiresHorizontalLeft[i] = new Wire<ElectronicMessage*>(so.str().c_str(), sc_time(1, SC_NS), parameters.internodeChannelStages, powerParams);
			messageWiresHorizontalRight[i] = new Wire<ElectronicMessage*>(so1.str().c_str(), sc_time(1, SC_NS), parameters.internodeChannelStages, powerParams);
		}
		
		if(parameters.torusType == ELEC_TORUS && i < networkSizeX)
		{
			messageWiresVerticalUp[i] = new Wire<ElectronicMessage*>(so2.str().c_str(), sc_time(1, SC_NS), parameters.longPathChannelStages, powerParams);
			messageWiresVerticalDown[i] = new Wire<ElectronicMessage*>(so3.str().c_str(), sc_time(1, SC_NS), parameters.longPathChannelStages, powerParams);
		}
		else 
		{
			messageWiresVerticalUp[i] = new Wire<ElectronicMessage*>(so2.str().c_str(), sc_time(1, SC_NS), parameters.internodeChannelStages, powerParams);
			messageWiresVerticalDown[i] = new Wire<ElectronicMessage*>(so3.str().c_str(), sc_time(1, SC_NS), parameters.internodeChannelStages, powerParams);
		}

	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{	
		std::stringstream so, so1, so2, so3;
		so << "creditwire_horizontalleft" << "<" << i << ">";
		so1 << "creditwire_horizontalright" << "<" << i << ">";
		so2 << "creditwire_verticalup" << "<" << i << ">";
		so3 << "creditwire_verticaldown" << "<" << i << ">";
		
		if(parameters.torusType == ELEC_TORUS && i % networkSizeX == 0)
		{
			creditWiresHorizontalLeft[i] = new Wire<int>(so.str().c_str(), sc_time(1, SC_NS), parameters.longPathChannelStages, powerParams);
			creditWiresHorizontalRight[i] = new Wire<int>(so1.str().c_str(), sc_time(1, SC_NS), parameters.longPathChannelStages, powerParams);
		}
		else
		{
			creditWiresHorizontalLeft[i] = new Wire<int>(so.str().c_str(), sc_time(1, SC_NS), parameters.internodeChannelStages, powerParams);
			creditWiresHorizontalRight[i] = new Wire<int>(so1.str().c_str(), sc_time(1, SC_NS), parameters.internodeChannelStages, powerParams);
		}
		
		if(parameters.torusType == ELEC_TORUS && i < networkSizeX)
		{
			creditWiresVerticalUp[i] = new Wire<int>(so2.str().c_str(), sc_time(1, SC_NS), parameters.longPathChannelStages, powerParams);
			creditWiresVerticalDown[i] = new Wire<int>(so3.str().c_str(), sc_time(1, SC_NS), parameters.longPathChannelStages, powerParams);
		}
		else 
		{
			creditWiresVerticalUp[i] = new Wire<int>(so2.str().c_str(), sc_time(1, SC_NS), parameters.internodeChannelStages, powerParams);
			creditWiresVerticalDown[i] = new Wire<int>(so3.str().c_str(), sc_time(1, SC_NS), parameters.internodeChannelStages, powerParams);
		}
	}
	
	
	// Establish Connections
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		// Top Side
		nodes[i]->outputPort[0]->bind(*verticalUpPathA[i]);
		nodes[i]->inputPort[0]->bind(*verticalDownPathA[i]);
		nodes[i]->creditOutputPort[0]->bind(*verticalUpCreditPathA[i]);
		nodes[i]->creditInputPort[0]->bind(*verticalDownCreditPathA[i]);
		
		
		messageWiresVerticalUp[i]->inputPort->bind(*verticalUpPathA[i]);
		messageWiresVerticalDown[i]->outputPort->bind(*verticalDownPathA[i]);
		creditWiresVerticalUp[i]->inputPort->bind(*verticalUpCreditPathA[i]);
		creditWiresVerticalDown[i]->outputPort->bind(*verticalDownCreditPathA[i]);
		
		
		messageWiresVerticalUp[i]->outputPort->bind(*verticalUpPathB[i]);
		messageWiresVerticalDown[i]->inputPort->bind(*verticalDownPathB[i]);
		creditWiresVerticalUp[i]->outputPort->bind(*verticalUpCreditPathB[i]);
		creditWiresVerticalDown[i]->inputPort->bind(*verticalDownCreditPathB[i]);
		
		
		// Bottom Side
		nodes[i]->inputPort[2]->bind(*verticalUpPathB[(((i / networkSizeX) + 1) % networkSizeY) * networkSizeX + (i % networkSizeX)]);
		nodes[i]->outputPort[2]->bind(*verticalDownPathB[(((i / networkSizeX) + 1) % networkSizeY) * networkSizeX + (i % networkSizeX)]);
		nodes[i]->creditInputPort[2]->bind(*verticalUpCreditPathB[(((i / networkSizeX) + 1) % networkSizeY) * networkSizeX + (i % networkSizeX)]);
		nodes[i]->creditOutputPort[2]->bind(*verticalDownCreditPathB[(((i / networkSizeX) + 1) % networkSizeY) * networkSizeX + (i % networkSizeX)]);
		
		
		// Left Side
		nodes[i]->outputPort[3]->bind(*horizontalLeftPathA[i]);
		nodes[i]->inputPort[3]->bind(*horizontalRightPathA[i]);
		nodes[i]->creditOutputPort[3]->bind(*horizontalLeftCreditPathA[i]);
		nodes[i]->creditInputPort[3]->bind(*horizontalRightCreditPathA[i]);
		
		messageWiresHorizontalLeft[i]->inputPort->bind(*horizontalLeftPathA[i]);
		messageWiresHorizontalRight[i]->outputPort->bind(*horizontalRightPathA[i]);
		creditWiresHorizontalLeft[i]->inputPort->bind(*horizontalLeftCreditPathA[i]);
		creditWiresHorizontalRight[i]->outputPort->bind(*horizontalRightCreditPathA[i]);
		
		messageWiresHorizontalLeft[i]->outputPort->bind(*horizontalLeftPathB[i]);
		messageWiresHorizontalRight[i]->inputPort->bind(*horizontalRightPathB[i]);
		creditWiresHorizontalLeft[i]->outputPort->bind(*horizontalLeftCreditPathB[i]);
		creditWiresHorizontalRight[i]->inputPort->bind(*horizontalRightCreditPathB[i]);
		
		// Right Side
		nodes[i]->inputPort[1]->bind(*horizontalLeftPathB[((i / networkSizeX) * networkSizeX) + (((i % networkSizeX) + 1) % networkSizeX)]);
		nodes[i]->outputPort[1]->bind(*horizontalRightPathB[((i / networkSizeX) * networkSizeX) + (((i % networkSizeX) + 1) % networkSizeX)]);
		nodes[i]->creditInputPort[1]->bind(*horizontalLeftCreditPathB[((i / networkSizeX) * networkSizeX) + (((i % networkSizeX) + 1) % networkSizeX)]);
		nodes[i]->creditOutputPort[1]->bind(*horizontalRightCreditPathB[((i / networkSizeX) * networkSizeX) + (((i % networkSizeX) + 1) % networkSizeX)]);
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		for(int j = 0; j < concentrationLevel; j++)
		{
				// Node Connection
			nodes[i]->inputPort[4+j]->bind(*inputPort[i * concentrationLevel + j]);
			nodes[i]->outputPort[4+j]->bind(*outputPort[i * concentrationLevel + j]);
			nodes[i]->creditInputPort[4+j]->bind(*inputCreditPort[i * concentrationLevel + j]);
			nodes[i]->creditOutputPort[4+j]->bind(*outputCreditPort[i * concentrationLevel + j]);
		}
	}
}

double ElectronicTorus_Topology::GetCumulativePower(){
	double totalEnergy = 0;
	for(int i = 0; i < networkSizeX * networkSizeY; i++){
		totalEnergy += nodes[i]->GetCumulativePower();
	}

	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		totalEnergy += messageWiresHorizontalLeft[i]->GetCumulativePower();
		totalEnergy +=  messageWiresHorizontalRight[i]->GetCumulativePower();
		totalEnergy +=  messageWiresVerticalUp[i]->GetCumulativePower();
		totalEnergy +=  messageWiresVerticalDown[i]->GetCumulativePower();
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{	
		totalEnergy +=  creditWiresHorizontalLeft[i]->GetCumulativePower();
		totalEnergy +=  creditWiresHorizontalRight[i]->GetCumulativePower();
		totalEnergy +=  creditWiresVerticalUp[i]->GetCumulativePower();
		totalEnergy +=  creditWiresVerticalDown[i]->GetCumulativePower();
	}
	return totalEnergy;
}

void ElectronicTorus_Topology::PrintStats(){
	double totalMessages = 0.0;
	for(int i = 0; i < networkSizeX * networkSizeY; i++){
		totalMessages += nodes[i]->GetTotalMessagesRouted();
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++){
		cout <<  "Router_" << i << "\tTotal Messages: " << nodes[i]->GetTotalMessagesRouted() << "\t" << (nodes[i]->GetTotalMessagesRouted() / totalMessages) * 100 << "%" << endl;
	}
}


ElectronicTorus_Topology::~ElectronicTorus_Topology()
{
	for(int i = 0; i < networkSizeX * networkSizeY * concentrationLevel; i++)
	{
		delete inputPort[i];
		delete outputPort[i];
		delete inputCreditPort[i];
		delete outputCreditPort[i];
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY * concentrationLevel; i++)
	{
		delete nodes[i];
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		delete horizontalLeftPathA[i];
		delete horizontalRightPathA[i];
		delete horizontalLeftCreditPathA[i];
		delete horizontalRightCreditPathA[i];
	}	
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		delete horizontalLeftPathB[i];
		delete horizontalRightPathB[i];
		delete horizontalLeftCreditPathB[i];
		delete horizontalRightCreditPathB[i];
	}	
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{	
		delete verticalUpPathA[i];
		delete verticalDownPathA[i];
		delete verticalUpCreditPathA[i];
		delete verticalDownCreditPathA[i];
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{	
		delete verticalUpPathB[i];
		delete verticalDownPathB[i];
		delete verticalUpCreditPathB[i];
		delete verticalDownCreditPathB[i];
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{
		delete messageWiresHorizontalLeft[i];
		delete messageWiresHorizontalRight[i];
		delete messageWiresVerticalUp[i];
		delete messageWiresVerticalDown[i];
	}
	
	for(int i = 0; i < networkSizeX * networkSizeY; i++)
	{	
		delete creditWiresHorizontalLeft[i];
		delete creditWiresHorizontalRight[i];
		delete creditWiresVerticalUp[i];
		delete creditWiresVerticalDown[i];
	}
}
