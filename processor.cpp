/*
 *  processor.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/11/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "processor.h"

#include "trafficgeneratorfactory.h"

using namespace std;
using namespace PhoenixSim;

Average_StatisticsMachine Processor::networkLatency;
Single_StatisticsMachine Processor::lastMessageTime;
bool Processor::once = false;


Processor::Processor(sc_module_name name, string p_generator, TrafficGeneratorParameters* p_parameters)
{
	inputPort = new sc_in<ElectronicMessage*>("input");
	outputPort = new sc_out<ElectronicMessage*>("output");
	
	trafficGenerator = TrafficGeneratorFactory::Instance()->CreateTrafficGenerator(p_generator);
	
	trafficGenerator->SetTrafficGeneratorParameters(p_parameters);
	
	SC_METHOD(Initialize);
	
	SC_METHOD(SendMessage);
	dont_initialize();
	sensitive << sendMessageEvent;
	
	SC_METHOD(MessageReceived);
	dont_initialize();
	sensitive << inputPort->value_changed();
	
	
}

Processor::~Processor()
{
	if(!once)
	{
		once = true;
		std::cout<<"Average Latency: "<<networkLatency.ComputeValue()<<endl;
		std::cout<<"Samples: "<<networkLatency.GetCount()<<endl;
		std::cout<<"Simulation Time: "<<lastMessageTime.ComputeValue()<<endl;
	}
	delete inputPort;
	delete outputPort;
}

void Processor::Initialize()
{
	ElectronicMessage* firstMessage = trafficGenerator->CreateFirstMessage();
	if(firstMessage != NULL)
	{
		sendMessageEvent.notify(trafficGenerator->WaitTime(firstMessage));
		messageQueue.push(firstMessage);
	}
}

void Processor::SendMessage()
{
	outputPort->write(messageQueue.front());
	messageQueue.pop();
	
	ElectronicMessage* nextMessage = trafficGenerator->MessageCreated();
	if(nextMessage != NULL)
	{
		sc_time val = trafficGenerator->WaitTime(nextMessage);
		sendMessageEvent.notify(val);
		messageQueue.push(nextMessage);
		
	}
}

void Processor::MessageReceived()
{
	//std::cout<<sc_time_stamp()<<" "<<this->name()<<" first flit -- received correct msg id = "<<inputPort->read()->messageId<<" from "<<inputPort->read()->sourceAddress<<" --> "<<endl;
	
	networkLatency.RecordValue((sc_time_stamp() - inputPort->read()->beginTime).to_seconds());
	lastMessageTime.RecordValue(sc_time_stamp().to_seconds());
	
	ElectronicMessage* nextMessage = trafficGenerator->MessageReceived(inputPort->read());
	if(nextMessage != NULL)
	{
		sendMessageEvent.notify(trafficGenerator->WaitTime(nextMessage));
		messageQueue.push(nextMessage);
	}
}

void Processor::MessageTransmitted()
{
	ElectronicMessage* nextMessage = trafficGenerator->MessageTransmitted();
	if(nextMessage != NULL)
	{
		sendMessageEvent.notify(trafficGenerator->WaitTime(nextMessage));
		messageQueue.push(nextMessage);
	}
}