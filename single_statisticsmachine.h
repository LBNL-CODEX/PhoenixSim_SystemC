/*
 *  single_statisticsmachine.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/13/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __SINGLE_STATISTICSMACHINE_H__
#define __SINGLE_STATISTICSMACHINE_H__

#include "statisticsmachine.h"

namespace PhoenixSim 
{
	class Single_StatisticsMachine : public StatisticsMachine
	{
	public:
		Single_StatisticsMachine();
		~Single_StatisticsMachine();
		
		void RecordValue(const double value);
		double ComputeValue() const;
		
	private:
		double currValue;
	};
}

#endif // __SINGLE_STATISTICSMACHINE_H__