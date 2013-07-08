/*
 *  average_statisticsmachine.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/30/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "average_statisticsmachine.h"

#include <iostream>

#include "systemc.h"

using namespace std;
using namespace PhoenixSim;

Average_StatisticsMachine::Average_StatisticsMachine()
:valueCount(0), valueSum(0.0)
{
	SetLabel("average");
}

Average_StatisticsMachine::~Average_StatisticsMachine()
{
}

void Average_StatisticsMachine::RecordValue(const double value)
{
	valueCount ++;
	valueSum += value;
}

double Average_StatisticsMachine::ComputeValue() const
{
	return valueSum / valueCount;
}

int Average_StatisticsMachine::GetCount() const
{
	return valueCount;
}