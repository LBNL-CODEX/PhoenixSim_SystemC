/*
 *  credit_interface.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/28/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *	Derived from Interface class. Includes ports for credit based flow control.
 */

#ifndef __CREDIT_INTERFACE_H__
#define __CREDIT_INTERFACE_H__

#include <list>
#include <vector>

#include "systemc.h"
#include "interface.h"
#include "electronicmessage.h"

namespace PhoenixSim 
{
	class Credit_Interface : public Interface
	{
	public:
		Credit_Interface(sc_module_name name, int p_numOfNetworkPorts, int p_bufferWidth);
		~Credit_Interface();
		
		std::vector< sc_in<int>* > creditInputPort;
		std::vector< sc_out<int>* > creditOutputPort;
		
	protected:
		const unsigned int bufferWidth;
		virtual void CreditReceived(int inputPortId) = 0;
	private:
	};
}

#endif // __CREDIT_INTERFACE_H__

