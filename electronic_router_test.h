/*
 *  electronic_router_test.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/24/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include <vector>
#include <list>
#include <time.h>
#include <stdlib.h>

#include "systemc.h"
#include "router.h"
#include "fixed_routingpolicy.h"

const int NUMINPUTS = 4;
const int NUMOUTPUTS = 4;
const int INPUTCONTROLWIDTH = 2; // log base 2 of NUMINPUTS

using namespace std;
using namespace PhoenixSim;

sc_event nothing;


SC_MODULE(stim2)
{
	sc_out< ElectronicMessage* > A;
	
	void SendMessage(int id, int dest, double waitafter, sc_time_unit unit = SC_NS)
	{
		ElectronicMessage* msg = new ElectronicMessage;
		msg->messageId = id;
		Address t;
		t.SetAddressLength(1);
		t.SetAddress(0,dest);
		msg->destinationAddress = t;
		msg->messageSize = 16;
		wait(waitafter, unit);
		A->write(msg);
	}
	
	void StimGen()
	{
		
		
		SendMessage(1, 0, 0.5);
		SendMessage(2, 1, 5);
		SendMessage(3, 0, 10);
		SendMessage(4, 3, 10);
		SendMessage(5, 3, 10);
		SendMessage(6, 2, 10);
		SendMessage(7, 1, 0.1);
		SendMessage(8, 0, 0.1);
		SendMessage(9, 0, 0.1);
		SendMessage(10, 0, 0.1);
		SendMessage(11, 3, 0.1);
		
		//wait(nothing);
		//sc_stop();
	}
	SC_CTOR(stim2)
	{
		SC_THREAD(StimGen);
		//sensitive << Clk.pos();
	}
};

SC_MODULE(stim3)
{
	sc_out< ElectronicMessage* > A;
	
	void SendMessage(int id, int dest, double waitafter, sc_time_unit unit = SC_NS)
	{
		ElectronicMessage* msg = new ElectronicMessage;
		msg->messageId = id;
		Address t;
		t.SetAddressLength(1);
		t.SetAddress(0,dest);
		msg->destinationAddress = t;
		msg->messageSize = 16;
		wait(waitafter, unit);
		A->write(msg);
	}
	
	void StimGen()
	{
		
		
		SendMessage(101, 2, 0.5);
		SendMessage(102, 1, 5);
		SendMessage(103, 0, 10);
		SendMessage(104, 3, 10);
		SendMessage(105, 3, 10);
		SendMessage(106, 2, 10);
		SendMessage(107, 1, 0.1);
		SendMessage(108, 0, 0.1);
		SendMessage(109, 0, 0.1);
		SendMessage(110, 0, 0.1);
		SendMessage(111, 3, 0.1);
		
		//wait(nothing);
		//sc_stop();
	}
	SC_CTOR(stim3)
	{
		SC_THREAD(StimGen);
		//sensitive << Clk.pos();
	}
};

SC_MODULE(stim4)
{
	sc_out< ElectronicMessage* > A;
	
	void SendMessage(int id, int dest, double waitafter, sc_time_unit unit = SC_NS)
	{
		ElectronicMessage* msg = new ElectronicMessage;
		msg->messageId = id;
		Address t;
		t.SetAddressLength(1);
		t.SetAddress(0,dest);
		msg->destinationAddress = t;
		msg->messageSize = 128;
		wait(waitafter, unit);
		A->write(msg);
	}
	
	void StimGen()
	{
		
		
		SendMessage(201, 0, 0.5);
		SendMessage(202, 1, 5);
		SendMessage(203, 0, 10);
		SendMessage(204, 3, 10);
		SendMessage(205, 3, 10);
		SendMessage(206, 2, 10);
		SendMessage(207, 1, 0.1);
		SendMessage(208, 0, 0.1);
		SendMessage(209, 0, 0.1);
		SendMessage(210, 0, 0.1);
		SendMessage(211, 3, 0.1);
		
		//wait(nothing);
		//sc_stop();
	}
	SC_CTOR(stim4)
	{
		SC_THREAD(StimGen);
		//sensitive << Clk.pos();
	}
};

SC_MODULE(stim5)
{
	sc_out< ElectronicMessage* > A;
	
	void SendMessage(int id, int dest, double waitafter, sc_time_unit unit = SC_NS)
	{
		ElectronicMessage* msg = new ElectronicMessage;
		msg->messageId = id;
		Address t;
		t.SetAddressLength(1);
		t.SetAddress(0,dest);
		msg->destinationAddress = t;
		msg->messageSize = 64;
		wait(waitafter, unit);
		A->write(msg);
	}
	
	void StimGen()
	{
		
		
		SendMessage(301, 0, 0.5);
		SendMessage(302, 1, 5);
		SendMessage(303, 0, 10);
		SendMessage(304, 3, 10);
		SendMessage(305, 3, 10);
		SendMessage(306, 2, 10);
		SendMessage(307, 1, 0.1);
		SendMessage(308, 0, 0.1);
		SendMessage(309, 0, 0.1);
		SendMessage(310, 0, 0.1);
		SendMessage(311, 3, 0.1);
		
		//wait(nothing);
		//sc_stop();
	}
	SC_CTOR(stim5)
	{
		SC_THREAD(StimGen);
		//sensitive << Clk.pos();
	}
};


void run_router () 
{
	// Global Simulation Parameters
	sc_set_time_resolution(100, SC_FS);
	std::cout<<"time resolution: "<<sc_get_time_resolution()<<endl;
	
	srand(time(NULL));
	
	
	// Top Level Signals
	
	vector< sc_buffer<ElectronicMessage*>* > data_in;
	vector< sc_buffer<ElectronicMessage*>* > data_out;
	
	vector< sc_buffer<int>* > credit_in;
	vector< sc_buffer<int>* > credit_out;
	
	data_in.resize(NUMINPUTS);
	data_out.resize(NUMOUTPUTS);
	
	credit_in.resize(NUMOUTPUTS);
	credit_out.resize(NUMINPUTS);
	
	//stim TestStim("Stimulus");
	stim2 TestStim2("PacketGenPort0");
	stim3 TestStim3("PacketGenPort1");
	stim4 TestStim4("PacketGenPort2");
	stim5 TestStim5("PacketGenPort3");
	
	
	Fixed_RoutingParameters* params = new Fixed_RoutingParameters;
	Router myRouter("myRouter", "fixed", params, NUMINPUTS, NUMOUTPUTS, 2, sc_time(1, SC_NS), 8, 32/2);
	
	
	
	for(int i = 0 ; i < NUMINPUTS ; i++)
	{
		data_in[i] = new sc_core::sc_buffer<ElectronicMessage*>;
		credit_out[i] = new sc_core::sc_buffer<int>;
		
		myRouter.inputPort[i]->bind(*data_in[i]);
		myRouter.creditOutputPort[i]->bind(*credit_out[i]);
	}
	
	
	for(int i = 0 ; i < NUMOUTPUTS ; i++)
	{
		data_out[i] = new sc_core::sc_buffer<ElectronicMessage*>;
		credit_in[i] = new sc_core::sc_buffer<int>;
		
		myRouter.outputPort[i]->bind(*data_out[i]);
		myRouter.creditInputPort[i]->bind(*credit_in[i]);
	}
	
	
	TestStim2.A(*(data_in[0]));
	TestStim3.A(*(data_in[1]));
	TestStim4.A(*(data_in[2]));
	TestStim5.A(*(data_in[3]));
	// Setup Trace Generator
	
	
	sc_trace_file *tf = sc_create_vcd_trace_file("dump");
	
	
	for(int i = 0; i < NUMINPUTS; i++)
	{
		std::stringstream so;
		so << "DataIn" << "<" << i << ">";
		sc_trace(tf, data_in[i]->read()->messageId, so.str());
	}
	
	for(int i = 0; i < NUMINPUTS; i++)
	{
		std::stringstream so;
		so << "Pop" << "<" << i << ">";
		sc_trace(tf, *(myRouter.sig_bufferPop[i]), so.str());
	}
	
	for(int i = 0; i < NUMINPUTS; i++)
	{
		std::stringstream so;
		so << "DataAvail" << "<" << i << ">";
		sc_trace(tf, *(myRouter.sig_dataAvailable[i]), so.str());
	}
	
	for(int i = 0; i < NUMINPUTS; i++)
	{
		std::stringstream so;
		so << "BufferToArbiter" << "<" << i << ">";
		sc_trace(tf, myRouter.sig_headerData[i]->read()->messageId, so.str());
	}
	
	for(int i = 0; i < NUMINPUTS; i++)
	{
		std::stringstream so;
		so << "ArbiterToSwitchData" << "<" << i << ">";
		sc_trace(tf, myRouter.buf_dataArbiterToSwitch[i]->read()->messageId, so.str());
	}
	
	for(int i = 0; i < NUMINPUTS; i++)
	{
		std::stringstream so;
		so << "ArbiterToSwitchControl" << "<" << i << ">";
		sc_trace(tf, myRouter.sig_controlArbiterToSwitch[i]->read(), so.str());
	}
	
	for(int i = 0; i < NUMOUTPUTS; i++)
	{
		std::stringstream so;
		so << "DataOut" << "<" << i << ">";
		sc_trace(tf, data_out[i]->read()->messageId, so.str());
	}
	
	for(int i = 0; i < NUMINPUTS; i++)
	{
		std::stringstream so;
		so << "Credit_Input" << "<" << i << ">";
		sc_trace(tf, credit_out[i]->read(), so.str());
	}
	
	for(int i = 0; i < NUMOUTPUTS; i++)
	{
		std::stringstream so;
		so << "Credit_Output" << "<" << i << ">";
		sc_trace(tf, credit_in[i]->read(), so.str());
	}
	
	
	
	
    sc_start();
	
	sc_close_vcd_trace_file(tf);
}
