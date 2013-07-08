/*
 *  sink_interface.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/29/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "sink_interface.h"

using namespace PhoenixSim;

Sink_Interface::Sink_Interface(sc_module_name name, int p_bufferWidth)
:Source_Interface(name, 1, 0, p_bufferWidth)
{
	spawnOptions = new sc_spawn_options;
	
	spawnOptions->spawn_method();
	spawnOptions->dont_initialize();
	spawnOptions->set_sensitivity(&(inputPort[0]->value_changed()));
	
	sc_spawn( sc_bind(&Sink_Interface::NetworkMessageReceived, this, 0) , "module_sink_interface" , spawnOptions);	
}

Sink_Interface::~Sink_Interface()
{
	delete spawnOptions;
}



void Sink_Interface::NetworkMessageReceived(int inputPortId)
{
	if(inputPort[inputPortId]->read()->messageId != -1)
	{
		creditOutputPort[inputPortId]->write(ceil(inputPort[inputPortId]->read()->messageSize / double(bufferWidth)));
	}
}

void Sink_Interface::CreditReceived(int inputPortId)
{
	// ignore
}