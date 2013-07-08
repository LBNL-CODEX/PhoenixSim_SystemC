/*
 *  trafficgenerator.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "trafficgenerator.h"

using namespace PhoenixSim;

int TrafficGenerator::currentMessageCount = 0;

TrafficGenerator::TrafficGenerator()
{
}

TrafficGenerator::~TrafficGenerator()
{
}

ElectronicMessage* TrafficGenerator::CreateFirstMessage()
{
	return NULL;
}

ElectronicMessage* TrafficGenerator::MessageReceived(ElectronicMessage* incomingMessage)
{
	delete incomingMessage;
	return NULL;
}

ElectronicMessage* TrafficGenerator::MessageTransmitted()
{
	return NULL;
}

ElectronicMessage* TrafficGenerator::MessageCreated()
{
	return NULL;
}

sc_time TrafficGenerator::WaitTime(ElectronicMessage* message)
{
	// Notice that this is zero time, when it might actually need a delta time.
	return sc_time(0, SC_NS);
}

int TrafficGenerator::GetUniqueId()
{
	return currentMessageCount++;
}
