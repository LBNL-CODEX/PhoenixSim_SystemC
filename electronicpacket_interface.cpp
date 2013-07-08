/*
 *  electronicpacket_interface.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "electronicpacket_interface.h"

#include <stdexcept>

using namespace std;
using namespace PhoenixSim;

ElectronicPacket_Interface::ElectronicPacket_Interface(sc_module_name name, sc_time p_clockPeriod, int p_bufferWidth, int p_bufferDepth, int p_maxPacketSize)
:Source_Interface(name, 1, 1, p_bufferWidth), maxPacketSize(p_maxPacketSize), clockPeriod(p_clockPeriod), bufferWidth(p_bufferWidth), requestSuccessful(false),
currentMessage(NULL), currentPacket(NULL), remainingData(0), creditsRemaining(p_bufferDepth), bufferDepth(p_bufferDepth), receivingMessage(NULL), transmissionComplete(false)
{
	creditReceivedOptions = new sc_spawn_options;
	
	creditReceivedOptions->spawn_method();
	creditReceivedOptions->dont_initialize();
	creditReceivedOptions->set_sensitivity(&(creditInputPort[0]->value_changed()));
	
	sc_spawn( sc_bind(&ElectronicPacket_Interface::CreditReceived, this, 0) , "module_credit_received" , creditReceivedOptions);	
	
	networkMessageReceivedOptions = new sc_spawn_options;
	
	networkMessageReceivedOptions->spawn_method();
	networkMessageReceivedOptions->dont_initialize();
	networkMessageReceivedOptions->set_sensitivity(&(inputPort[0]->value_changed()));
	
	sc_spawn( sc_bind(&ElectronicPacket_Interface::NetworkMessageReceived, this, 0) , "module_networkmessage_received" , networkMessageReceivedOptions);	
	
	processorDataReceivedOptions = new sc_spawn_options;
	
	processorDataReceivedOptions->spawn_method();
	processorDataReceivedOptions->dont_initialize();
	processorDataReceivedOptions->set_sensitivity(&(processorDataInputPort[0]->value_changed()));
	
	sc_spawn( sc_bind(&ElectronicPacket_Interface::ProcessorDataReceived, this, 0) , "module_processordata_received" , processorDataReceivedOptions);	
	
	SC_METHOD(Controller);
	dont_initialize();
	sensitive << controllerEvent;
}

ElectronicPacket_Interface::~ElectronicPacket_Interface()
{
	
}

void ElectronicPacket_Interface::NetworkMessageReceived(int inputPortId)
{
	if(inputPort[inputPortId]->read() != NULL)
	{
		if(receivingMessage == NULL)
		{
			receivingMessage = inputPort[inputPortId]->read();
		}
		else if(receivingMessage != inputPort[inputPortId]->read())
		{
			creditOutputPort[inputPortId]->write(ceil(receivingMessage->messageSize / double(bufferWidth)));
			
			if(receivingMessage->messageComplete)
			{
				processorDataOutputPort[0]->write(receivingMessage);
			}
			else 
			{
				delete receivingMessage;
			}

			
			receivingMessage = inputPort[inputPortId]->read();
		}

		
	}
	else 
	{
		if(receivingMessage != NULL)
		{
			creditOutputPort[inputPortId]->write(ceil(receivingMessage->messageSize / double(bufferWidth)));
			
			if(receivingMessage->messageComplete)
			{
				processorDataOutputPort[0]->write(receivingMessage);
			}
			else 
			{
				delete receivingMessage;
			}
			receivingMessage = NULL;
		}
	}

}

void ElectronicPacket_Interface::CreditReceived(int inputPortId)
{
	creditsRemaining += creditInputPort[inputPortId]->read();
	controllerEvent.notify(clockPeriod);
	
	assert(bufferDepth >= creditsRemaining);
	//std::cout<<this->name()<<" adding credits "<<creditsRemaining<<endl;
}

void ElectronicPacket_Interface::ProcessorDataReceived(int inputPortId)
{
	if(processorDataInputPort[inputPortId]->read() != NULL)
	{
		messageQueue.push(processorDataInputPort[inputPortId]->read());
		if(messageQueue.size() > 100000)
		{
			throw runtime_error("message Queue reached 100000 messages, time to quit.");
		}
		controllerEvent.notify(clockPeriod);
	}
}

void ElectronicPacket_Interface::Controller()
{
	if(requestSuccessful || Request())
	{
		if(Prepare())
		{
			channelReadyTime = Send() + sc_time_stamp();
			controllerEvent.notify(channelReadyTime - sc_time_stamp());
		}
		else if(transmissionComplete)
		{
			Complete();
		}

	}
}

bool ElectronicPacket_Interface::Request()
{
	if(currentMessage == NULL && messageQueue.size() > 0)
	{
		currentMessage = messageQueue.front();
		messageQueue.pop();
		remainingData = currentMessage->messageSize;
		
		requestSuccessful = true;
		return true;
	}
	
	return false;
}

bool ElectronicPacket_Interface::Prepare()
{
	if(remainingData > 0 && sc_time_stamp() >= channelReadyTime)
	{		
		// Make sure there is enough room for at least a single bit payload
		assert(maxPacketSize - DATA_HEADER_SIZE_ > 0);
		
		// Check if there are currently enough credits
		if( ceil(min(maxPacketSize, remainingData + DATA_HEADER_SIZE_) / double(bufferWidth)) > creditsRemaining)
		{
			return false;
		}
		
		currentPacket = new ElectronicMessage;
		(*currentPacket) = (*currentMessage);
		currentPacket->sequenceId = currentSequenceId++;
		currentPacket->messageComplete = remainingData <= maxPacketSize - DATA_HEADER_SIZE_;
		currentPacket->messageSize = min(maxPacketSize, remainingData + DATA_HEADER_SIZE_);
		
		
		return true;
	}
	return false;
}

sc_time ElectronicPacket_Interface::Send()
{
	sc_time waitTime = ceil(currentPacket->messageSize / double(bufferWidth)) * clockPeriod;	
	creditsRemaining -= ceil(currentPacket->messageSize / double(bufferWidth));
	assert(creditsRemaining >= 0);
	remainingData -= currentPacket->messageSize - DATA_HEADER_SIZE_;
	transmissionComplete = remainingData == 0;
	outputPort[0]->write(currentPacket);
	currentPacket = NULL;
	return waitTime;
}

void ElectronicPacket_Interface::Complete()
{
	transmissionComplete = false;
	//std::cout<<sc_time_stamp()<<" "<<this->name()<<" whole message sent in "<< currentSequenceId <<" pieces" << currentMessage->sourceAddress <<"-->"<<currentMessage->destinationAddress <<endl;
	currentMessage = NULL;
	requestSuccessful = false;
	currentSequenceId = 0;
	controllerEvent.notify(clockPeriod);
}