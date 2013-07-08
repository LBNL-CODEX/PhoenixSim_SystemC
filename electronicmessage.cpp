/*
 *  electronicmessage.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/6/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "electronicmessage.h"

#include "systemc.h"

using namespace std;
using namespace PhoenixSim;


ElectronicMessage::ElectronicMessage()
{
	messageId = -1;
	sequenceId = 0;
	messageType = "default";
	messageSize = 0;
	beginTime = sc_time_stamp();
	data = NULL;
	messageComplete = false;
}

bool ElectronicMessage::operator ==(const ElectronicMessage & rhs) const
{
	if(messageId == -1 && rhs.messageId == -1)
	{
		return true;
	}
	else
	{
		return messageId == rhs.messageId && sequenceId == rhs.sequenceId && sourceAddress == rhs.sourceAddress && destinationAddress == rhs.destinationAddress && 
		virtualChannelId == rhs.virtualChannelId && beginTime == rhs.beginTime && endTime == rhs.endTime && messageComplete == rhs.messageComplete && data == rhs.data &&
		messageType == rhs.messageType && messageSize == rhs.messageSize;
	}
}

bool ElectronicMessage::operator !=(const ElectronicMessage & rhs) const
{
	return !((*this) == rhs);
}

ElectronicMessage& ElectronicMessage::operator =(const ElectronicMessage & rhs)
{
	messageId = rhs.messageId;
	sequenceId = rhs.sequenceId;
	messageType = rhs.messageType;
	messageSize = rhs.messageSize;
	sourceAddress = rhs.sourceAddress;
	destinationAddress = rhs.destinationAddress;
	virtualChannelId = rhs.virtualChannelId;
	beginTime = rhs.beginTime;
	endTime = rhs.endTime;
	messageComplete = rhs.messageComplete;
	data = rhs.data;
	return *this;	
}

bool ElectronicMessage::IsNullMessage() const
{
	return messageId == -1;
}

void PhoenixSim::sc_trace (sc_trace_file *tf, const ElectronicMessage &v, const string &NAME)
{
	sc_trace(tf, v.messageId, NAME + ".messageId");
	//sc_trace(tf, v.sourceId, NAME + ".sourceId");
	//sc_trace(tf, v.destinationId, NAME + ".destinationId");
	sc_trace(tf, v.virtualChannelId, NAME + ".virtualChannelId");
}

void PhoenixSim::sc_trace (sc_trace_file *tf, const ElectronicMessage* v, const string &NAME)
{
	//
	if(v == NULL)
	{
		
	}
	else 
	{
		sc_trace(tf, long(v), NAME);
	}

}

ostream& PhoenixSim::operator<<(ostream& os, ElectronicMessage const &v)
{
	if(v.messageId == -1)
	{
		return os<<"ElectronicMessage: NULL";
	}
	else
	{
		return os<<"ElectronicMessage: id="<<v.messageId; //<<" sourceId="<<v.sourceId<<" destinationId="<<v.destinationId;
	}
}