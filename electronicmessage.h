/*
 *  electronicmessage.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/6/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ELECTRONICMESSAGE_H__
#define __ELECTRONICMESSAGE_H__

#include <string>
#include <ostream>

#include "systemc.h"
#include "address.h"

namespace PhoenixSim
{
	class ElectronicMessage;
	
	void sc_trace (sc_trace_file *tf, const ElectronicMessage &v, const std::string &NAME);
	void sc_trace (sc_trace_file *tf, const ElectronicMessage* v, const std::string &NAME);
	std::ostream& operator <<(std::ostream& os, ElectronicMessage const &v);
	
	class Payload
	{
		
	};
	
	class ElectronicMessage
	{
		public:
			ElectronicMessage();
		
			
			int messageId;		// value of -1 will be considered a NULL message
								// enumeration of message ID begins at 0
			
			int sequenceId;
			bool messageComplete;
		
			std::string messageType;
			int messageSize;	// size of message, in units of bits 
			
			Address sourceAddress;
			Address destinationAddress;
			int virtualChannelId;

			sc_time beginTime;
			sc_time endTime;
		
			Payload* data;
			
			
			bool operator ==(const ElectronicMessage & rhs) const;
			bool operator !=(const ElectronicMessage & rhs) const;
			virtual ElectronicMessage& operator =(const ElectronicMessage& rhs);
		
			friend void sc_trace (sc_trace_file *tf, const ElectronicMessage &v, const std::string &NAME);
			friend void sc_trace (sc_trace_file *tf, const ElectronicMessage *v, const std::string &NAME);
			friend std::ostream& operator <<(std::ostream& os, ElectronicMessage const &v);	
			
			bool IsNullMessage() const;
	};
	
	
}


#endif // __ELECTRONICMESSAGE_H__


