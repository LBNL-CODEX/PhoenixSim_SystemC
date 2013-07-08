/*
 *  router_parameters.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/6/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ROUTER_PARAMETERS_H__
#define __ROUTER_PARAMETERS_H__

#include "systemc.h"

namespace PhoenixSim
{
	struct Router_Parameters
	{
		unsigned int networkSizeX;
		unsigned int networkSizeY;
		
		unsigned int numOfGrants;
		unsigned int numOfInputPorts;
		unsigned int numOfOutputPorts;
		sc_time clockPeriod;
		unsigned int bufferDepth;
		unsigned int bufferWidth;
		unsigned int numOfVirtualChannels;
	}
}

#endif // __ROUTER_PARAMETERS_H__

