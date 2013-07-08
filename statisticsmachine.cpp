/*
 *  statisticsmachine.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 6/30/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "statisticsmachine.h"

using namespace std;
using namespace PhoenixSim;

StatisticsMachine::StatisticsMachine()
:label("none")
{
}

StatisticsMachine::~StatisticsMachine()
{
}

void StatisticsMachine::SetName(string newName)
{
	name = newName;
}

void StatisticsMachine::SetLabel(string newLabel)
{
	label = newLabel;
}


ostream& PhoenixSim::operator <<(ostream& os, StatisticsMachine const &stat)
{
	os << stat.name << "," << stat.ComputeValue() << "," << stat.label ;
	return os;
}