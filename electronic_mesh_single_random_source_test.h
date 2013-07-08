/*
 *  electronic_mesh_single_random_source_test.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/29/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *	This is to test network's ability to handle interface credit return.
 */

#include <vector>
#include <list>
#include <time.h>

#include <stdlib.h>

#include "systemc.h"

#include "electronicmesh_topology.h"
#include "random_interface.h"
#include "terminate_interface.h"

const int SIZE_X = 4;
const int SIZE_Y = 4;

const double MEANARRIVALTIME = 1e-9;
const int MESSAGESIZE = 32;
using namespace std;
using namespace PhoenixSim;


void run_net_single_node () 
{
	// Global Simulation Parameters
	sc_set_time_resolution(100, SC_FS);
	std::cout<<"time resolution: "<<sc_get_time_resolution()<<endl;
	
	srand(time(NULL));
	
	
	// Top Level Signals
	
	vector< sc_buffer<ElectronicMessage>* > data_in;
	vector< sc_buffer<ElectronicMessage>* > data_out;
	
	vector< sc_buffer<int>* > credit_in;
	vector< sc_buffer<int>* > credit_out;
	
	
	data_in.resize(SIZE_X * SIZE_Y);
	data_out.resize(SIZE_X * SIZE_Y);
	
	credit_in.resize(SIZE_X * SIZE_Y);
	credit_out.resize(SIZE_X * SIZE_Y);
	
	
	ElectronicMesh_Topology network("myNetwork", SIZE_X, SIZE_Y);
	vector<Credit_Interface*> node(SIZE_X * SIZE_Y);
	
	node[0] = new Random_Interface("Node_zero", 0, SIZE_X * SIZE_Y, MEANARRIVALTIME, MESSAGESIZE, 32);
	for(int i = 1; i < SIZE_X * SIZE_Y; i++)
	{
		std::stringstream so;
		so << "Node" << "<" << i << ">";
		node[i] = new Terminate_Interface(so.str().c_str(), 32);
	}
	
	for(int i = 0 ; i < SIZE_X * SIZE_Y ; i++)
	{
		data_in[i] = new sc_core::sc_buffer<ElectronicMessage>;
		data_out[i] = new sc_core::sc_buffer<ElectronicMessage>;
		credit_in[i] = new sc_core::sc_buffer<int>;
		credit_out[i] = new sc_core::sc_buffer<int>;
		
		network.inputPort[i]->bind(*data_in[i]);
		network.outputPort[i]->bind(*data_out[i]);
		network.inputCreditPort[i]->bind(*credit_in[i]);
		network.outputCreditPort[i]->bind(*credit_out[i]);
		
		node[i]->outputPort[0]->bind(*data_in[i]);
		node[i]->inputPort[0]->bind(*data_out[i]);
		
		node[i]->creditInputPort[0]->bind(*credit_out[i]);
		node[i]->creditOutputPort[0]->bind(*credit_in[i]);
	}
	
	
	sc_trace_file *tf = sc_create_vcd_trace_file("electronic_mesh");
	
	
	
	
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		std::stringstream so, so1;
		so << "DataIn" << "<" << i << ">";
		sc_trace(tf, data_in[i]->read().messageId, so.str());
	}
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		std::stringstream so, so1;
		so1 << "CreditIn" << "<" << i << ">";
		sc_trace(tf, credit_in[i]->read(), so1.str());
	}
	
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		std::stringstream so, so1;
		so << "DataOut" << "<" << i << ">";
		sc_trace(tf, data_out[i]->read().messageId, so.str());
	}
	for(int i = 0; i < SIZE_X * SIZE_Y; i++)
	{
		std::stringstream so, so1;	
		so1 << "CreditOut" << "<" << i << ">";
		sc_trace(tf, credit_out[i]->read(), so1.str());
	}
	
	sc_trace(tf,network.horizontalRightPath[7]->read().messageId, "path");
	
	
    sc_start();
	
	sc_close_vcd_trace_file(tf);
}
