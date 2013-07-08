/*
 *  statisticsmachine.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/30/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */


#ifndef __STATISTICSMACHINE_H__
#define __STATISTICSMACHINE_H__

#include <iostream>
#include <string>

namespace PhoenixSim 
{
	class StatisticsMachine
	{
		public:
			StatisticsMachine();
			~StatisticsMachine();
			
			virtual void RecordValue(const double value) = 0;
			virtual double ComputeValue() const = 0;
		
			void SetName(std::string newName);
		
			friend std::ostream& operator<<(std::ostream& os, PhoenixSim::StatisticsMachine const &stat);
		protected:
			void SetLabel(std::string newLabel);
		
		private:
		
		
			std::string name;
			std::string label;
	};
	
	
	std::ostream& operator <<(std::ostream& os, StatisticsMachine const &stat);
}



#endif // __STATISTICSMACHINE_H__