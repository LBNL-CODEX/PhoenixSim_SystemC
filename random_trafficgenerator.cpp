/*
 *  random_trafficgenerator.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/8/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "random_trafficgenerator.h"

using namespace std;
using namespace PhoenixSim;

boost::mt19937 Random_TrafficGenerator::gen;


Random_TrafficGeneratorParameters::Random_TrafficGeneratorParameters()
:numOfMessages(-1)
{
}




Random_TrafficGenerator::Random_TrafficGenerator()
:rng_interarrivaltime(NULL), numOfMessagesSent(0)
{	
	
	// this parameter is only used for NOCS style traffic
	waitForPreviousMessage = false;
	firstMessage = true;
}

Random_TrafficGenerator::~Random_TrafficGenerator()
{
}


void Random_TrafficGenerator::SetTrafficGeneratorParameters(TrafficGeneratorParameters *p_parameters)
{
	Random_TrafficGeneratorParameters* parameters = static_cast<Random_TrafficGeneratorParameters*>(p_parameters);
	
	addressSpace = parameters->addressSpace;
	interarrivalTime = parameters->interarrivalTime;
	messageSize = parameters->messageSize;
	nodeAddress = parameters->nodeAddress;
	totalNumOfMessagesToSend = parameters->numOfMessages;
	
	boost::exponential_distribution<> dist_interarrivaltime(1/interarrivalTime.to_seconds());
	boost::uniform_int<> dist_nodeDestination(0, addressSpace.GetAddress(1) - 1);
	boost::uniform_int<> dist_concDestination(0, addressSpace.GetAddress(0) - 1);
    rng_interarrivaltime = new boost::variate_generator<boost::mt19937&, boost::exponential_distribution<> >(gen, dist_interarrivaltime);
	rng_nodeDestination = new boost::variate_generator<boost::mt19937&, boost::uniform_int<> >(gen, dist_nodeDestination);
	rng_concDestination = new boost::variate_generator<boost::mt19937&, boost::uniform_int<> >(gen, dist_concDestination);
}

ElectronicMessage* Random_TrafficGenerator::CreateFirstMessage()
{
	return MessageCreated();
}

ElectronicMessage* Random_TrafficGenerator::MessageReceived(ElectronicMessage* incomingMessage)
{
	assert(incomingMessage != NULL);
	
	ElectronicMessage * returnMessage = NULL;
	
	
	
	if(waitForPreviousMessage)
	{
		if(incomingMessage->messageType == "data")
		{
			ElectronicMessage* msg = new ElectronicMessage;
			msg->messageId = GetUniqueId();
			msg->messageType = "ack";
			msg->sourceAddress = nodeAddress;
			msg->destinationAddress = incomingMessage->sourceAddress;
			msg->messageSize = 1;
			
			//numOfMessagesSent++;
			
			returnMessage = msg;
		}
		else // "ack"
		{
			if(totalNumOfMessagesToSend != -1 && totalNumOfMessagesToSend <= numOfMessagesSent)
			{
				returnMessage = NULL;
			}
			else
			{
				numOfMessagesSent++;
				returnMessage = SendDataMessage();
			}
		}
	}
	
	delete incomingMessage;
	
	return returnMessage;
}

ElectronicMessage* Random_TrafficGenerator::MessageCreated()
{
	if(totalNumOfMessagesToSend != -1 && totalNumOfMessagesToSend <= numOfMessagesSent)
	{
		return NULL;
	}
	
	if(!waitForPreviousMessage || firstMessage)
	{
		firstMessage = false;

		numOfMessagesSent++;
		   
		return SendDataMessage();
	}
	else 
	{
		return NULL;
	}

}

sc_time	Random_TrafficGenerator::WaitTime(ElectronicMessage* message)
{
	return sc_time((*rng_interarrivaltime)(), SC_SEC);
}

Address Random_TrafficGenerator::RandomDestination()
{
	Address destinationAddress;
	destinationAddress.SetAddressLength(2);
	destinationAddress.SetAddress(0, (*rng_concDestination)());
	destinationAddress.SetAddress(1, (*rng_nodeDestination)());
	
	return destinationAddress;
}

ElectronicMessage* Random_TrafficGenerator::SendDataMessage()
{
	ElectronicMessage* msg = new ElectronicMessage;
	msg->messageId = GetUniqueId();
	msg->messageType = "data";
	msg->sourceAddress = nodeAddress;
	msg->destinationAddress = RandomDestination();
	msg->messageSize = messageSize;

	return msg;
}