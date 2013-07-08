/*
 *  routercrossbar.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/10/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "routercrossbar.h"

#include <sstream>

using namespace PhoenixSim;

RouterCrossbar::RouterCrossbar(sc_module_name name, int p_numOfInputPorts, int p_numOfOutputPorts, sc_time p_clockPeriod, const COrionPowerParams& powerParams)
: sc_module(name), numOfInputPorts(p_numOfInputPorts), numOfOutputPorts(p_numOfOutputPorts), controlInputPort(p_numOfInputPorts), dataInputPort(p_numOfInputPorts), dataOutputPort(p_numOfOutputPorts), clockPeriod(p_clockPeriod)
{
	for (int i = 0; i < numOfInputPorts; i++) 
	{
		std::stringstream so ,so1;
		so << "controlInputPort" << "<" << i << ">";
		controlInputPort[i] = new sc_in<int>(so.str().c_str());
		so1 << "dataInputPort" << "<" << i << ">";
		dataInputPort[i] = new sc_in<ElectronicMessage*>(so1.str().c_str());
	}
	
	for (int i = 0; i < numOfOutputPorts; i++) 
	{
		std::stringstream so;
		so << "dataOutputPort" << "<" << i << ">";
		dataOutputPort[i] = new sc_out<ElectronicMessage*>(so.str().c_str());
		//dataOutputPort[i]->initialize(ElectronicMessage());
	}
	
	SC_METHOD(UpdateCrossbar);
	dont_initialize();
	sensitive << updateEvent;
	SC_METHOD(SetUpdateDelay);
	dont_initialize();
	for(int i = 0; i < numOfInputPorts; i++)
	{
		sensitive << controlInputPort[i]->value_changed();
		sensitive << dataInputPort[i]->value_changed();
	}

	power_ = new ORION_Crossbar();

	int model = MATRIX_CROSSBAR;
	int ins = numOfInputPorts;
	int outs = numOfOutputPorts;
	int in_seg = 0; //used by segmented crossbar
	int out_seg = 0; //used by segmented crossbar
	int data_width = 32;
	int degree = 4; //used by multree crossbar
	int connect_type = TRISTATE_GATE;
	int trans_type = powerParams.m_transType;
	double in_len = 0; //use 0 if unknown, ORION will figure it out
	double out_len = 0;

	int technology = powerParams.m_technology;

	double voltage = powerParams.m_voltage;
	double frequency =powerParams.m_frequency;

	ORION_Tech_Config *conf = new ORION_Tech_Config(technology, trans_type,
			voltage, frequency);

	power_->init(model, ins, outs, in_seg, out_seg, data_width, degree,
			connect_type, trans_type, in_len, out_len, conf);

#ifdef __REPORT_STATIC_POWER_AT_STARTUP__
	std::cout << "Reporting crossbar static POWER: " << power_->report_static_power() << " W \n";
#endif

}

RouterCrossbar::~RouterCrossbar()
{

	double power = power_->report();
#ifdef __REPORT_TOTAL_ENERGY_AT_TEARDOWN__
	std::cout.precision(10);
	std::cout << "Reporting final ENERGY for crossbar: " << power << " J \n";
#endif
	delete power_;

	for (int i = 0; i < numOfInputPorts; i++) 
	{
		delete controlInputPort[i];
		delete dataInputPort[i];
		delete dataOutputPort[i];
	}
}

void RouterCrossbar::UpdateCrossbar()
{	
	//std::cout<<sc_time_stamp()<<" "<<this->name()<<" updating crossbar"<<endl;
	bool writeOutput;
	
	for(int i = 0; i < numOfOutputPorts; i++)
	{
		writeOutput = false;
		
		for(int j = 0; j < numOfInputPorts; j++)
		{
			// if this input routing matches the output then do this
			if(controlInputPort[j]->read() == i)
			{
				if(dataOutputPort[i]->read() != dataInputPort[j]->read())
				{
					dataOutputPort[controlInputPort[j]->read()]->write(dataInputPort[j]->read());
					power_->record(1, j, i, dataInputPort[j]->read()->messageSize);  //input
					power_->record(0, j, i, dataInputPort[j]->read()->messageSize);  //output
					messagesRouted +=1;
				}
				writeOutput = true;
			}
		}
		if(!writeOutput)
		{
			dataOutputPort[i]->write(NULL);
		}
	}
	
	

}

void RouterCrossbar::SetUpdateDelay()
{
	updateEvent.notify(clockPeriod);
}
