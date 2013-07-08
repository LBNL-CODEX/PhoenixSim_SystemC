/*
 *  electronic_torus_test.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/1/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include <vector>
#include <list>
#include <string>
#include <time.h>
#include <sstream>
#include <stdlib.h>

#include "systemc.h"

#include "electronictorus_topology.h"
#include "electronicpacket_interface.h"

#include "processor.h"
#include "random_trafficgenerator.h"
#include "lbltrace_trafficgenerator.h"
#include "lbltrace_ackall_trafficgenerator.h"

#include "configuration.h"

const int SIZE_X = 4;
const int SIZE_Y = 4;

const int CONC_LEVEL = 4;
//const PhoenixSim::TorusType NETWORKTYPE = PhoenixSim::ELEC_MESH;
const PhoenixSim::TorusType NETWORKTYPE = PhoenixSim::ELEC_FOLDEDTORUS;
const double MEANARRIVALTIME = 0;
const int MESSAGESIZE = 96*8;
 
const std::string FILE_NAME = "./traces/madbench4.64";
//const std::string FILE_NAME = "../../traces/gtc_trace_phases_large.64";
//const std::string FILE_NAME = "./traces/paratec_trace_phases.64";
//const std::string FILE_NAME = "../../traces/cactus_trace_phases_large.64";
//const std::string FILE_NAME = "../../traces/data.64";

const std::string INI_FILE = "./config/param.ini";

using namespace std;
using namespace PhoenixSim;


void run_torus () 
{
	
	Configuration config;
	config.ParseFile(INI_FILE);
	
//	return;
	
	// Global Simulation Parameters
	sc_set_time_resolution(100, SC_FS);
	std::cout<<"time resolution: "<<sc_get_time_resolution()<<endl;
	
	srand(time(NULL));
	
	
	// Top Level Signals
	
	vector< sc_buffer<ElectronicMessage*>* > data_in;
	vector< sc_buffer<ElectronicMessage*>* > data_out;
	
	vector< sc_buffer<int>* > credit_in;
	vector< sc_buffer<int>* > credit_out;
	
	
	data_in.resize(SIZE_X * SIZE_Y * CONC_LEVEL);
	data_out.resize(SIZE_X * SIZE_Y * CONC_LEVEL);
	
	credit_in.resize(SIZE_X * SIZE_Y * CONC_LEVEL);
	credit_out.resize(SIZE_X * SIZE_Y * CONC_LEVEL);
	
	
	ElectronicTorus_Parameters params;
	params.networkSizeX = SIZE_X;
	params.networkSizeY = SIZE_Y;
	params.torusType = NETWORKTYPE;
	params.concentrationType = CONC_NODE;
	params.concentrationLevel = CONC_LEVEL;
	params.internodeChannelStages = 0;
	params.longPathChannelStages = 0;
	params.numOfGrants = 4;
	params.clockPeriod = sc_time(1,SC_NS);
	params.bufferDepth = 16;
	params.bufferWidth = 128;
	params.numOfVirtualChannels = 1;
	params.maxPacketSize = 256;

	COrionPowerParams  powerParms;	//defaults set by constructor are fine
		
	ElectronicTorus_Topology network("myNetwork", params, powerParms);
	vector<Source_Interface*> node(SIZE_X * SIZE_Y * CONC_LEVEL);
	vector<Processor*> proc(SIZE_X * SIZE_Y * CONC_LEVEL);
	
	vector< sc_buffer<ElectronicMessage*>* > to_proc;
	vector< sc_buffer<ElectronicMessage*>* > from_proc;
	
	to_proc.resize(SIZE_X * SIZE_Y * CONC_LEVEL);
	from_proc.resize(SIZE_X * SIZE_Y * CONC_LEVEL);
	
	/*/
	 Trace_Interface hi("hi", 0, 16, 1, 2, FILE_NAME, 1, 1);
	std::cout<<"end"<<endl;
	return; /**/
	Random_TrafficGeneratorParameters *trafficparams = new Random_TrafficGeneratorParameters;
	trafficparams->addressSpace.SetAddressLength(2);
	trafficparams->addressSpace.SetAddress(0, CONC_LEVEL);
	trafficparams->addressSpace.SetAddress(1, SIZE_X * SIZE_Y);
	
	
	trafficparams->nodeAddress.SetAddressLength(2);
	
	trafficparams->interarrivalTime = sc_time(MEANARRIVALTIME, SC_SEC);
	trafficparams->messageSize = MESSAGESIZE;
	trafficparams->numOfMessages = 100;
	
	LBLTrace_TrafficGeneratorParameters *traceparam = new LBLTrace_TrafficGeneratorParameters;
	traceparam->fileName = FILE_NAME;
	traceparam->networkSize = SIZE_X * SIZE_Y * CONC_LEVEL;
	traceparam->nodeAddress.SetAddressLength(2);
	traceparam->addressSpace.SetAddressLength(2);
	traceparam->addressSpace.SetAddress(0, CONC_LEVEL);
	traceparam->addressSpace.SetAddress(1, SIZE_X * SIZE_Y);
	
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		for(int j = 0; j < CONC_LEVEL; j++)
		{
			std::stringstream so,so1;
			so << "Node" << "<" << j << ","<< i << ">";
			//Use this as interface 
			node[i * CONC_LEVEL + j] = new ElectronicPacket_Interface(so.str().c_str(), params.clockPeriod, params.bufferWidth, params.bufferDepth, params.maxPacketSize);
			so1 << "Proc" << "<" << j << ","<< i << ">";
			
			//nodes represent network endpoints. consist of
			//interfaces and helper functions.
			// Interface to processor and handle
			// ElectronicMessages

			//This informs endpoints what address each DMA / node
			//should be mapped into.  The NoC should own this
			//mapping
			trafficparams->nodeAddress.SetAddress(0, j);
			trafficparams->nodeAddress.SetAddress(1, i);
			
			//proc[i * CONC_LEVEL + j] = new Processor(so1.str().c_str(), "random", trafficparams);
			
			
			traceparam->nodeAddress.SetAddress(0, j);
			traceparam->nodeAddress.SetAddress(1, i);
			
			proc[i * CONC_LEVEL + j] = new Processor(so1.str().c_str(), "lbltrace", traceparam);
			
			to_proc[i * CONC_LEVEL + j] = new sc_core::sc_buffer<ElectronicMessage*>;
			from_proc[i * CONC_LEVEL + j] = new sc_core::sc_buffer<ElectronicMessage*>;
			
			proc[i * CONC_LEVEL + j]->inputPort->bind(*to_proc[i * CONC_LEVEL + j]);
			proc[i * CONC_LEVEL + j]->outputPort->bind(*from_proc[i * CONC_LEVEL + j]);
			
			node[i * CONC_LEVEL + j]->processorDataOutputPort[0]->bind(*to_proc[i * CONC_LEVEL + j]);
			node[i * CONC_LEVEL + j]->processorDataInputPort[0]->bind(*from_proc[i * CONC_LEVEL + j]);
		}
	}
	
	delete trafficparams;
	
	for(int i = 0 ; i < SIZE_X * SIZE_Y ; i++)
	{		
		for(int j = 0; j < CONC_LEVEL; j++)
		{
			data_in[i * CONC_LEVEL + j] = new sc_core::sc_buffer<ElectronicMessage*>;
			data_out[i * CONC_LEVEL + j] = new sc_core::sc_buffer<ElectronicMessage*>;
			credit_in[i * CONC_LEVEL + j] = new sc_core::sc_buffer<int>;
			credit_out[i * CONC_LEVEL + j] = new sc_core::sc_buffer<int>;
			
			network.inputPort[i * CONC_LEVEL + j]->bind(*data_in[i * CONC_LEVEL + j]);
			network.outputPort[i * CONC_LEVEL + j]->bind(*data_out[i * CONC_LEVEL + j]);
			network.inputCreditPort[i * CONC_LEVEL + j]->bind(*credit_in[i * CONC_LEVEL + j]);
			network.outputCreditPort[i * CONC_LEVEL + j]->bind(*credit_out[i * CONC_LEVEL + j]);
			
			node[i * CONC_LEVEL + j]->outputPort[0]->bind(*data_in[i * CONC_LEVEL + j]);
			node[i * CONC_LEVEL + j]->inputPort[0]->bind(*data_out[i * CONC_LEVEL + j]);
			
			node[i * CONC_LEVEL + j]->creditInputPort[0]->bind(*credit_out[i * CONC_LEVEL + j]);
			node[i * CONC_LEVEL + j]->creditOutputPort[0]->bind(*credit_in[i * CONC_LEVEL + j]);
		}
	}
	
	
	sc_trace_file *tf = sc_create_vcd_trace_file("electronic_mesh");
	
	
	
	
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		for(int j = 0; j < CONC_LEVEL; j++)
		{
			std::stringstream so;
			so << "DataIn" << "<" << j << ","<< i << ">";
			sc_trace(tf, data_in[i]->read(), so.str());
		}
	}
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		for(int j = 0; j < CONC_LEVEL; j++)
		{
			std::stringstream so;
			so << "CreditIn" << "<" << j << ","<< i << ">";
			sc_trace(tf, credit_in[i]->read(), so.str());
		}
	}
	
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		for(int j = 0; j < CONC_LEVEL; j++)
		{
			std::stringstream so;
			so << "DataOut" << "<" << j << ","<< i << ">";
			sc_trace(tf, data_out[i]->read(), so.str());
		}
	}
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		for(int j = 0; j < CONC_LEVEL; j++)
		{
			std::stringstream so;	
			so << "CreditOut" << "<" << j << ","<< i << ">";
			sc_trace(tf, credit_out[i]->read(), so.str());
		}
	}
	
	
	
    sc_start();
	
	sc_close_vcd_trace_file(tf);
	
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		for(int j = 0; j < CONC_LEVEL; j++)
		{
			
			delete node[i * CONC_LEVEL + j];
			delete proc[i * CONC_LEVEL + j];
		}
	}
}
