/*
 *  routerbuffer.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/10/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include <iostream>

#include "routerbuffer.h"

using namespace PhoenixSim;

RouterBuffer::RouterBuffer(sc_module_name name, sc_time p_clockPeriod, int p_bufferDepth, int p_bufferWidth, const COrionPowerParams& powerParams)
:sc_module(name), clockPeriod(p_clockPeriod), bufferDepth(p_bufferDepth), bufferWidth(p_bufferWidth)
{
	creditsRemaining = bufferDepth;
	
	pop = new sc_in<bool>("port_pop");
	ready = new sc_out<bool>("port_ready");
	inputPort = new sc_in<ElectronicMessage*>("port_dataInput");
	outputPort = new sc_out<ElectronicMessage*>("port_dataOutput");

	int buf_type = SRAM;
	int is_fifo = true;
	int n_read_port = 1;
	int n_write_port = 1;
	int n_entry = p_bufferDepth;
	int line_width = p_bufferWidth;
	int outdrv = true;

	int technology = powerParams.m_technology;
	int trans_type = powerParams.m_transType
;
	double voltage = powerParams.m_voltage;
	double frequency = powerParams.m_frequency;

	ORION_Tech_Config *conf = new ORION_Tech_Config(technology, trans_type,
			voltage, frequency);
	ORION_Array_Info* info = new ORION_Array_Info();
	info->init(buf_type, is_fifo, n_read_port, n_write_port, n_entry,
			line_width, outdrv, conf);

	power_ = new ORION_Array();
	power_->init(info);

	energy_ = power_->stat_static_power();

#ifdef __REPORT_STATIC_POWER_AT_STARTUP__
	std::cout << "Reporting buffer static POWER: " << power_->stat_static_power() << " W \n";
#endif

	SC_METHOD(PushOntoQueue);
		dont_initialize();
		sensitive << inputPort->value_changed();
	SC_METHOD(ModifyOutput);
		dont_initialize();
		sensitive << dataReady;
	SC_METHOD(PopOffQueue);
		dont_initialize();
		sensitive << pop->value_changed();


}

RouterBuffer::~RouterBuffer()
{
#ifdef __REPORT_TOTAL_ENERGY_AT_TEARDOWN__
	std::cout << "Reporting final ENERGY for buffer: " << energy_ << " J \n";
#endif
	delete power_;

	delete pop;
	delete ready;
	delete inputPort;
	delete outputPort;
}

void RouterBuffer::PushOntoQueue()
{
	if(inputPort->read() != NULL)
	{
		if(creditsRemaining >= ceil(inputPort->read()->messageSize / double(bufferWidth)))
		{
			creditsRemaining -= ceil(inputPort->read()->messageSize / double(bufferWidth));
			
			//std::cout<<sc_time_stamp()<<" "<<this->name()<<" has "<<creditsRemaining<<" remaining, pushed msg "<< inputPort->read()->messageId<<endl;
			
			//std::cout<<sc_time_stamp()<<" "<<this->name()<<" pushing x"<<inputPort->read()<<" id:"<<inputPort->read()->messageId<<endl;
			energy_ += power_->stat_energy(0, ceil(inputPort->read()->messageSize / double(bufferWidth)), 0);

			queuedItems.push_back(inputPort->read());
			
			
			dataReady.notify(clockPeriod);
			
		}
		else
		{
			std::cout<<sc_time_stamp()<<" "<<this->name()<<" Buffer full, message lost "<< inputPort->read()->messageId<<endl;
		}
	}
}

void RouterBuffer::ModifyOutput()
{	
	if(pop->read())
	{
		creditsRemaining += ceil(queuedItems.front()->messageSize / double(bufferWidth));
		//std::cout<<sc_time_stamp()<<" "<<this->name()<<" has "<<creditsRemaining<<" remaining, popped msg "<< queuedItems.front().messageId<<endl;
		queuedItems.pop_front();
	}
	
	if(queuedItems.size() == 0)
	{
		outputPort->write(NULL);
		ready->write(false);
	}
	else
	{
		energy_ += power_->stat_energy(ceil(queuedItems.front()->messageSize / double(bufferWidth)), 0, 0);

		ready->write(true);
		outputPort->write(queuedItems.front());


	}

}

void RouterBuffer::PopOffQueue()
{		
	dataReady.notify(clockPeriod);
}



