/*
 *  routerarbiter.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/14/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "routerarbiter.h"

#include <sstream>

#include "routingpolicyfactory.h"

using namespace std;
using namespace PhoenixSim;

RouterArbiter::RouterArbiter(sc_module_name name, string p_routingPolicy, RoutingParameters* p_routingParameters, int p_numOfInputPorts, int p_numOfOutputPorts, int p_numOfGrants, sc_time p_clockPeriod, int p_bufferDepth, int p_bufferWidth, const COrionPowerParams& powerParams, int p_numOfVirtualChannels)
: sc_module(name), routerId(p_routingParameters->routerId), numOfInputPorts(p_numOfInputPorts), numOfOutputPorts(p_numOfOutputPorts), numOfGrants(p_numOfGrants), clockPeriod(p_clockPeriod), bufferDepth(p_bufferDepth), bufferWidth(p_bufferWidth), numOfVirtualChannels(p_numOfVirtualChannels)
{
	needToWriteOutput = false;
	nextBufferToCheck = 0;
	routingPolicy = RoutingPolicyFactory::Instance()->CreateRoutingPolicy(p_routingPolicy);	
	routingPolicy->SetRoutingParameters(p_routingParameters);
	
	inputConnectedTo.resize(numOfInputPorts,-1);
	outputConnectedTo.resize(numOfOutputPorts,-1);
	inputEndTime.resize(numOfInputPorts,sc_time(0,SC_NS));
	outputEndTime.resize(numOfOutputPorts,sc_time(0,SC_NS));
	
	varDataOutput.resize(numOfInputPorts);
	varCrossbarControlOutput.resize(numOfInputPorts, -1);
	
	bufferPopOutputPort.resize(numOfInputPorts);
	crossbarControlOutputPort.resize(numOfInputPorts);
	bufferDataAvailablePort.resize(numOfInputPorts);
	dataInputPort.resize(numOfInputPorts);
	dataOutputPort.resize(numOfOutputPorts);
	creditInputPort.resize(numOfOutputPorts);
	creditOutputPort.resize(numOfInputPorts);
	
	remainingCredits.resize(numOfInputPorts * numOfVirtualChannels, bufferDepth);
	
	for(int i = 0; i < numOfInputPorts; i++)
	{
		stringstream so ,so1,so2,so3,so4,so5;
		so << "port_bufferPop" << "<" << i << ">";
		bufferPopOutputPort[i] = new sc_out<bool>(so.str().c_str());
		bufferPopOutputPort[i]->initialize(false);
		so1 << "port_crossbarControl" << "<" << i << ">";
		crossbarControlOutputPort[i] = new sc_out<int>(so1.str().c_str());
		crossbarControlOutputPort[i]->initialize(-1);
		so2 << "port_bufferDataAvailable" << "<" << i << ">";
		bufferDataAvailablePort[i] = new sc_in<bool>(so2.str().c_str());
		so3 << "port_dataRead" << "<" << i << ">";
		dataInputPort[i] = new sc_in<ElectronicMessage*>(so3.str().c_str());
		so4 << "port_creditOutput" << "<" << i << ">";
		creditOutputPort[i] = new sc_out<int>(so4.str().c_str());
	}
	
	for(int i = 0; i < numOfOutputPorts; i++)
	{
		stringstream so, so1;
		so << "port_dataWrite" << "<" << i << ">";
		dataOutputPort[i] = new sc_out<ElectronicMessage*>(so.str().c_str());
		so1 << "port_creditInput" << "<" << i << ">";
		creditInputPort[i] = new sc_in<int>(so1.str().c_str());
	}

	int model = RR_ARBITER;
	int ff_model = NEG_DFF;
	int req_width = (numOfInputPorts - 1) * numOfVirtualChannels;

	int technology = powerParams.m_technology;
	int trans_type = powerParams.m_transType;
	double voltage =powerParams.m_voltage;
	double frequency = powerParams.m_frequency;

	ORION_Tech_Config *conf = new ORION_Tech_Config(technology, trans_type,
			voltage, frequency);

	double length = numOfInputPorts * p_bufferWidth * conf->PARM("CrsbarCellWidth");

	double st_pwr = 0;
	for (int i = 0; i < numOfInputPorts; i++) {
		power_[i] = new ORION_Arbiter();

		power_[i]->init(model, ff_model, req_width, length, conf);
		st_pwr += power_[i]->report_static_power();
	}



#ifdef __REPORT_STATIC_POWER_AT_STARTUP__
	std::cout << "Reporting arbiter static POWER: " << st_pwr << " W \n";
#endif


	SC_METHOD(RoutePacket);
	dont_initialize();
	sensitive << updateEvent;
	
	SC_METHOD(SetUpdateDelay);
	dont_initialize();
	sensitive << popEvent;
	for(int i = 0; i < numOfInputPorts; i++)
	{
		sensitive << dataInputPort[i]->value_changed();
	}
	
	//SC_METHOD(receiveCredit);
	//dont_initialize();
	//for(int i = 0; i < numOfOutputPorts; i++)
	//{
//		sensitive << creditInputPort[i]->value_changed();
//	}
	
	spawnOptions.resize(numOfInputPorts);
	
	for(int i = 0; i < numOfInputPorts; i++)
	{
		stringstream so;
		so << "method_receivedCredit" << "<" << i << ">";
		
		spawnOptions[i] = new sc_spawn_options;
		
		spawnOptions[i]->spawn_method();
		spawnOptions[i]->dont_initialize();
		spawnOptions[i]->set_sensitivity(&(creditInputPort[i]->value_changed()));
		
		sc_spawn( sc_bind(&RouterArbiter::receiveCredit, this, i) , so.str().c_str() , spawnOptions[i]);
		//dont_initialize();
		//sensitive << creditInputPort[i]->value_changed();
	}
	
}

double RouterArbiter::GetCumulativePower(){
	
	double energy = 0;
	for(int i = 0; i < numOfInputPorts; i++){
		energy += power_[i]->report_static_power();
		energy += power_[i]->report();
	}
	return energy;
}

RouterArbiter::~RouterArbiter()
{

	double energy = 0;
	for(int i = 0; i < numOfInputPorts; i++)
	{
		delete bufferPopOutputPort[i];
		delete crossbarControlOutputPort[i];
		delete bufferDataAvailablePort[i];
		delete dataInputPort[i];
		delete spawnOptions[i];
		delete creditOutputPort[i];

		energy += power_[i]->report();
		delete power_[i];
	}
	
#ifdef __REPORT_TOTAL_ENERGY_AT_TEARDOWN__
	std::cout << "Reporting final ENERGY for arbiter: " << energy << " J \n";
#endif

	for(int i = 0; i < numOfOutputPorts; i++)
	{
		
		delete dataOutputPort[i];
		delete creditInputPort[i];
	}
}

list<int>* RouterArbiter::selectBuffer()

{
	list<int>* bufferList = new list<int>;
	//round robin, only pick non-empty slots
	for(int i = 0; i < numOfInputPorts; i++)
	{
		if(bufferDataAvailablePort[(i + nextBufferToCheck) % numOfInputPorts]->read())
		// if there is data to be transmitted, and the input port of the switch is not occupied
		{
			bufferList->push_back((i + nextBufferToCheck) % numOfInputPorts);
			if(bufferList->size() == numOfGrants)
			{
				nextBufferToCheck = (i + 1 + nextBufferToCheck) % numOfInputPorts;
				break;
			}
		}
	}
	
	return bufferList;
}

void RouterArbiter::RoutePacket()
{	
	needToWriteOutput = false;
	
	// from the SetUpdateDelay, now set latched output
	for(int i = 0; i < numOfInputPorts; i++)
	{
		if(varDataOutput[i] != dataOutputPort[i]->read())
		{
			dataOutputPort[i]->write(varDataOutput[i]);
		}
		if(varCrossbarControlOutput[i] != crossbarControlOutputPort[i]->read())
		{
			crossbarControlOutputPort[i]->write(varCrossbarControlOutput[i]);
		}
	}
	
	bool atLeastOneAvailable = false;
	
	for(int i = 0; i < numOfInputPorts; i++)
	{
		if(bufferDataAvailablePort[i]->read())
		{
			atLeastOneAvailable = true;
			break;
		}
	}
	
	if(atLeastOneAvailable)
	{
		popEvent.notify(SC_ZERO_TIME);
	}
	else 
	{
		for(int i = 0; i < numOfInputPorts; i++)
		{
			if(inputConnectedTo[i] != -1)
			{
				popEvent.notify(inputEndTime[i] - sc_time_stamp());
			}
		}
	}
}

void RouterArbiter::SetUpdateDelay()
{	
	if(needToWriteOutput)
	{
		return;
	}
	needToWriteOutput = true;
	
	//do all the logic, and immediately output combination output

	
	// first, remove switch reservations for packets that have been sent through
	for(int i = 0; i < numOfInputPorts; i++)
	{
		if(inputConnectedTo[i] != -1 && sc_time_stamp() >= inputEndTime[i])
		{
			creditOutputPort[i]->write(ceil(varDataOutput[i]->messageSize / double(bufferWidth)));
			varDataOutput[i] = NULL;
			//crossbarControlOutputPort[i]->write(-1);
			varCrossbarControlOutput[i] = -1;
			outputConnectedTo[inputConnectedTo[i]] = -1;
			inputConnectedTo[i] = -1;
		}
	}
	
	
	// select packets to consider
	pair<int, int> outputId;
	int outputIndex;
	sc_time endTime;
	list<int>* packetsToRoute = selectBuffer();
	
	
	//std::cout<<"length "<<packetsToRoute->size()<<endl;
	
	for(int i = 0; i < numOfInputPorts; i++)
	{
		bufferPopOutputPort[i]->write(false);
		
	}
	
	// make a decision
	for(list<int>::iterator it = packetsToRoute->begin(); it != packetsToRoute->end(); it++)
		//for(int i = 0; i < numOfInputPorts; i++)
	{
		bool okayToPop = false;
		//dataOutputPort[*it]->write(dataInputPort[*it]->read());
		//varDataOutput[*it] = dataInputPort[*it]->read();
		
		if(inputConnectedTo[*it] == -1)
		{
			outputId = routingPolicy->Route(dataInputPort[*it]->read(), *it, 1, remainingCredits);
			outputIndex = outputId.first + outputId.second * numOfVirtualChannels;
			
			if(outputId.first != -1 && outputConnectedTo[outputIndex] == -1)
			{
				if(remainingCredits[outputIndex] >= ceil(dataInputPort[*it]->read()->messageSize / double(bufferWidth)))
				{
					okayToPop = true;
					varCrossbarControlOutput[*it] = outputIndex;
					
					varDataOutput[*it] = dataInputPort[*it]->read();
					inputConnectedTo[*it] = outputIndex;
					outputConnectedTo[outputIndex] = *it;
					endTime = sc_time_stamp() + clockPeriod * ceil(double(dataInputPort[*it]->read()->messageSize)/bufferWidth);
					inputEndTime[*it] = endTime;
					outputEndTime[outputIndex] = endTime;
					
					remainingCredits[outputIndex] -= ceil(varDataOutput[*it]->messageSize / double(bufferWidth));
					//std::cout<<sc_time_stamp()<<" "<<this->name()<<": outcredits["<<outputPortId<<"]--> "<<remainingCredits[outputPortId] + ceil(varDataOutput[*it]->messageSize / double(bufferWidth))<<"-"<<ceil(varDataOutput[*it]->messageSize / double(bufferWidth))<<"="<<remainingCredits[outputPortId]<<" <= "<<bufferDepth<<endl;

				}
				else 
				{
					//if(sc_time_stamp() > sc_time(1945, SC_NS))
					//std::cout<<sc_time_stamp()<<" "<<this->name()<<" flow control on port "<<*it<<"-->"<<outputPortId<<endl;
				}

			}
		}
		bufferPopOutputPort[*it]->write(okayToPop);

		//this is number of new requests, number of grants.
		// maybe not totally accurate, because these are not necessarily new
		// requests, but ariter power is tiny any way.
		power_[*it]->record(1, (okayToPop) ? 1 : 0);

	}
	delete packetsToRoute;
	updateEvent.notify(clockPeriod);
	
	// if there is a packet in a buffer (data available signal), then route after 1 clock cycle
}

void RouterArbiter::receiveCredit(int portId)
{
	//std::cout<<"%% messageid:"<<dataInputPort[*it]->read().messageId<<endl;
	remainingCredits[portId] += creditInputPort[portId]->read();

	//std::cout<<sc_time_stamp()<<" "<<this->name()<<": credits["<<portId<<"]--> "<<remainingCredits[portId] - creditInputPort[portId]->read()<<"+"<<creditInputPort[portId]->read()<<"="<<remainingCredits[portId]<<" <= "<<bufferDepth<<endl;

	assert(remainingCredits[portId] <= bufferDepth);
	
}
