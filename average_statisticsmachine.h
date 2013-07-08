/*
 *  average_statisticsmachine.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/30/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __AVERAGE_STATISTICSMACHINE_H__
#define __AVERAGE_STATISTICSMACHINE_H__

#include "statisticsmachine.h"

namespace PhoenixSim 
{
	class Average_StatisticsMachine : public StatisticsMachine
	{
		public:
			Average_StatisticsMachine();
			~Average_StatisticsMachine();
			
			void RecordValue(const double value);
			double ComputeValue() const;
		
			int GetCount() const;
		private:
			int valueCount;
			double valueSum;
	};
}

#endif // __AVERAGE_STATISTICSMACHINE_H__