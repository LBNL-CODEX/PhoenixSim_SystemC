/*
 *  electronicmesh_xy_routingpolicy.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/24/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *  Performs dimension-order routing on a grid topology
 *
 */

#ifndef __ELECTRONICMESH_XY_ROUTINGPOLICY_H__
#define __ELECTRONICMESH_XY_ROUTINGPOLICY_H__

#include <string>
#include "routingpolicy.h"
#include "routingpolicyfactory.h"

namespace PhoenixSim
{	
	class ElectronicMesh_XY_RoutingParameters : public RoutingParameters
	{
		public:
			int networkSizeX;
			int networkSizeY;
	};
	
	class ElectronicMesh_XY_RoutingPolicy : public RoutingPolicy
	{
		public:
			void SetRoutingParameters(RoutingParameters *p_parameters);
			std::pair<int, int> Route(ElectronicMessage* currentMessage, int inputPort, int inputVirtualChannel, const std::vector<int> &routerCredits);
		
		private:
			ElectronicMesh_XY_RoutingParameters routingParameters;
	};
	
	// Register with RoutingPolicyFactory
	namespace
	{
		RoutingPolicy* CreateElectronicMeshXYPolicy()
		{
			return new ElectronicMesh_XY_RoutingPolicy;
		}
		const std::string identifier = "electronicmesh_xy";
		
		const bool registered = RoutingPolicyFactory::Instance()->RegisterRoutingPolicy(CreateElectronicMeshXYPolicy, identifier);
	}
}

#endif // __ELECTRONICMESH_XY_ROUTINGPOLICY_H__