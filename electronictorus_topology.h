/*
 *  electronictorus_topology.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/5/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *	This file implements all three electronic mesh style topologies:
 *		1) Mesh
 *		2) Torus
 *		3) Folded Torus
 *	
 *	(1) Mesh and (2) Torus are implemented in similar ways. The node
 *	enumeration is done by row, then by column. Torus differs from Mesh in that
 *	it contains additional channels that cross the length of the chip, producing
 *	the rings. The extra length of the cross-chip channels can be accounted for 
 *	by introducing an additional delay through the 'longPathChannelStages' 
 *	member variable. In the case of (1) Mesh and (3) Folded Torus, all channels 
 *	assume the 'internodeChannelStages' delay.
 *
 *	When using (3) Folded Torus, the user should be aware that the node topology
 *	is exactly the same as (2) Torus, just differing in which channel delay
 *	parameters are used. The physical layout of the Folded Torus of a nodes 
 *	would not be layed out in the same way as a regular torus since Folded Torus
 *	nodes characteristically skips their nearest neighbors to connect to their
 *	second degree neighbors. This representation however is still correct since
 *	the nodes can be mapped in a way the replicates a nearest neighbor connected
 *	topology.
 */

#ifndef __ELECTRONICTORUS_TOPOLOGY_H__
#define __ELECTRONICTORUS_TOPOLOGY_H__

#include <vector>

#include "systemc.h"
#include "electronicmessage.h"
#include "router.h"
#include "wire.h"

namespace PhoenixSim 
{
	enum TorusType
	{
		ELEC_MESH,
		ELEC_TORUS,
		ELEC_FOLDEDTORUS
	};
	
	enum ConcentrationType
	{
		CONC_NONE,	// No concentration (default)
		CONC_NODE,	// Concentration located at network node
		
		// not implemented
		CONC_EXT	// Concentration at a secondary router, external to main network
	};
	
	struct ElectronicTorus_Parameters
	{
		int networkSizeX;
		int networkSizeY;
		
		TorusType torusType;
		ConcentrationType concentrationType;
		int concentrationLevel;
		
		int internodeChannelStages;
		int longPathChannelStages;
		
		int numOfGrants;
		sc_time clockPeriod;
		int bufferDepth;
		int bufferWidth;
		int maxPacketSize;
		int numOfVirtualChannels;
	};
	
	SC_MODULE(ElectronicTorus_Topology)
	{
		
		SC_HAS_PROCESS(ElectronicTorus_Topology);
		
		ElectronicTorus_Topology(sc_module_name name, const ElectronicTorus_Parameters & parameters, const COrionPowerParams& powerParams);
		~ElectronicTorus_Topology();
		
		std::vector< sc_in<ElectronicMessage*>* > inputPort;
		std::vector< sc_out<ElectronicMessage*>* > outputPort;
		
		std::vector< sc_in<int>* > inputCreditPort;
		std::vector< sc_out<int>* > outputCreditPort;
		
		std::vector<Router*> nodes;
		
		std::vector<Router*> externalNodes;
		
		std::vector<Wire<ElectronicMessage*>* > messageWiresHorizontalLeft;
		std::vector<Wire<ElectronicMessage*>* > messageWiresHorizontalRight;
		
		std::vector<Wire<ElectronicMessage*>* > messageWiresVerticalUp;
		std::vector<Wire<ElectronicMessage*>* > messageWiresVerticalDown;
		
		std::vector<Wire<int>* > creditWiresHorizontalLeft;
		std::vector<Wire<int>* > creditWiresHorizontalRight;
		
		std::vector<Wire<int>* > creditWiresVerticalUp;
		std::vector<Wire<int>* > creditWiresVerticalDown;

		
	public:
		unsigned int networkSizeX;
		unsigned int networkSizeY;
		double GetCumulativePower();
		void	PrintStats();
		
		ConcentrationType concentrationType;
		unsigned int	concentrationLevel;
		
		std::vector< sc_buffer<ElectronicMessage*>* > horizontalLeftPathA;
		std::vector< sc_buffer<ElectronicMessage*>* > horizontalRightPathA;
		std::vector< sc_buffer<ElectronicMessage*>* > verticalUpPathA;
		std::vector< sc_buffer<ElectronicMessage*>* > verticalDownPathA;
		
		std::vector< sc_buffer<int>* > horizontalLeftCreditPathA;
		std::vector< sc_buffer<int>* > horizontalRightCreditPathA;
		std::vector< sc_buffer<int>* > verticalUpCreditPathA;
		std::vector< sc_buffer<int>* > verticalDownCreditPathA;
		
		std::vector< sc_buffer<ElectronicMessage*>* > horizontalLeftPathB;
		std::vector< sc_buffer<ElectronicMessage*>* > horizontalRightPathB;
		std::vector< sc_buffer<ElectronicMessage*>* > verticalUpPathB;
		std::vector< sc_buffer<ElectronicMessage*>* > verticalDownPathB;
		
		std::vector< sc_buffer<int>* > horizontalLeftCreditPathB;
		std::vector< sc_buffer<int>* > horizontalRightCreditPathB;
		std::vector< sc_buffer<int>* > verticalUpCreditPathB;
		std::vector< sc_buffer<int>* > verticalDownCreditPathB;
	};
}

#endif // __ELECTRONICTORUS_TOPOLOGY_H__

