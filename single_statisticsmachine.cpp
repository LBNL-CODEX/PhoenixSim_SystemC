/*
 *  single_statisticsmachine.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/13/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "single_statisticsmachine.h"

using namespace std;
using namespace PhoenixSim;

Single_StatisticsMachine::Single_StatisticsMachine()
{
	SetLabel("single");
}

Single_StatisticsMachine::~Single_StatisticsMachine()
{
}

void Single_StatisticsMachine::RecordValue(const double value)
{
	currValue = value;
}

double Single_StatisticsMachine::ComputeValue() const
{
	return currValue;
}
