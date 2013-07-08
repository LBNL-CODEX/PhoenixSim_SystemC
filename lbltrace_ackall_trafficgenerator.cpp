/*
 *  lbltrace_ackall_trafficgenerator.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/19/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "lbltrace_ackall_trafficgenerator.h"

#include <vector>

#include "electronicmessage.h"

using namespace std;
using namespace PhoenixSim;

map<Address, int> LBLTrace_AckAll_TrafficGenerator::kernelMap;
map<int, Address> LBLTrace_AckAll_TrafficGenerator::processorMap;
bool LBLTrace_AckAll_TrafficGenerator::isInitialized = false;

map<int, queue<int> > LBLTrace_AckAll_TrafficGenerator::messagePhaseQueue;
map<int, queue<int> > LBLTrace_AckAll_TrafficGenerator::messageDestinationKernelQueue;
map<int, queue<int> > LBLTrace_AckAll_TrafficGenerator::messageSizeQueue;

int LBLTrace_AckAll_TrafficGenerator::lastPhase;


boost::mt19937 LBLTrace_AckAll_TrafficGenerator::gen;


LBLTrace_AckAll_TrafficGeneratorParameters::LBLTrace_AckAll_TrafficGeneratorParameters()
{
}

LBLTrace_AckAll_TrafficGenerator::LBLTrace_AckAll_TrafficGenerator()
:ackCount(0), waitForAck(false), traceDone(false), startNextPhase(false), waitingForMessageAck(false)
{	
}

LBLTrace_AckAll_TrafficGenerator::~LBLTrace_AckAll_TrafficGenerator()
{
}


void LBLTrace_AckAll_TrafficGenerator::SetTrafficGeneratorParameters(TrafficGeneratorParameters *p_parameters)
{
	
	LBLTrace_AckAll_TrafficGeneratorParameters* parameters = static_cast<LBLTrace_AckAll_TrafficGeneratorParameters*>(p_parameters);
	
	
	
	addressSpace = parameters->addressSpace;
	networkSize = parameters->networkSize;
	nodeAddress = parameters->nodeAddress;
	fileName = parameters->fileName;
	
	
	InitializeDataStructures();

	if(nodeKernelId == 0)
	{
		std::cout<<"starting phase: "<<currentPhase<<endl;
		std::cout<<"last phase: "<<lastPhase<<endl;
	}
}

void LBLTrace_AckAll_TrafficGenerator::InitializeDataStructures()
{
	int nextMessagePhase, nextMessageSourceKernel, nextMessageDestinationKernel, nextMessageSize;
	
	traceFile.open(fileName.c_str(), ios::in);
	assert(!traceFile.fail());
	
	traceFile >> currentPhase;
	
	if(!isInitialized)
	{
		RandomKernelInterfaceMapping();
		
		traceFile.seekg(ios::beg);
		
		
		do
		{
			traceFile >> nextMessagePhase;
			traceFile.seekg(1, ios::cur);
			traceFile >> nextMessageSourceKernel;
			traceFile.seekg(1, ios::cur);
			traceFile >> nextMessageDestinationKernel;
			traceFile.seekg(1, ios::cur);
			traceFile >> nextMessageSize;
			traceFile.seekg(1, ios::cur);
			
			if(!traceFile.eof() && !traceFile.fail())
			{
				
				messagePhaseQueue[nextMessageSourceKernel].push(nextMessagePhase);
				messageDestinationKernelQueue[nextMessageSourceKernel].push(nextMessageDestinationKernel);
				messageSizeQueue[nextMessageSourceKernel].push(nextMessageSize*8);
				
				lastPhase = nextMessagePhase;
			}
			else 
			{			
				break;
			}
			
		}while(true);
		
		isInitialized = true;
		
			
	}
	//std::cout<<"nodeAddress: "<<nodeAddress<<" has messages:"<<messagePhaseQueue[kernelMap[nodeAddress]].size()<<endl;
	
	nodeKernelId = kernelMap[nodeAddress];
	
	traceFile.close();
	//std::cout<<"I am processor "<<nodeAddress<<" mapped to kernel "<< kernelMap[nodeAddress] << " which connected to proc: "<<processorMap[kernelMap[nodeAddress]]<<endl;
}

void LBLTrace_AckAll_TrafficGenerator::RandomKernelInterfaceMapping()
{
	int index;
	Address currAddress;
	currAddress.SetAddressLength(2);
	
	vector<int> kernelAssignments;
	
	for(int i = 0; i < networkSize; i++)
	{
		kernelAssignments.push_back(i);
	}
	
	for(int i = 0; i < addressSpace.GetAddress(1) ; i++)
	{
		currAddress.SetAddress(1,i);
		for(int j = 0; j < addressSpace.GetAddress(0) ; j++)
		{
			
			currAddress.SetAddress(0,j);
			
			
			boost::uniform_int<> dist(0, kernelAssignments.size() - 1);
			
			boost::variate_generator<boost::mt19937&, boost::uniform_int<> > rng(gen, dist);
			
			
			index = rng();
			kernelMap[currAddress] = kernelAssignments[index];
			processorMap[kernelAssignments[index]] = currAddress;
			kernelAssignments.erase(kernelAssignments.begin() + index);
		}
	}
	
	
}

ElectronicMessage* LBLTrace_AckAll_TrafficGenerator::CreateFirstMessage()
{
	return MessageCreated();
}

ElectronicMessage* LBLTrace_AckAll_TrafficGenerator::MessageReceived(ElectronicMessage* incomingMessage)
{
	// if this node is the master node
	ElectronicMessage *returnMessage = NULL;
	
	//if(kernelMap[incomingMessage->destinationAddress] == 0)
	//std::cout<<nodeAddress<<" "<<kernelMap[nodeAddress]<<" received "<<incomingMessage->messageType<<" from "<<incomingMessage->sourceAddress<<":"<<kernelMap[incomingMessage->sourceAddress]<<endl;
	
	if(incomingMessage->messageType == "data")
	{
		
		if(incomingMessage->destinationAddress == nodeAddress)
		{
			returnMessage = SendMessageAckMessage(incomingMessage->sourceAddress);
		}
		else 
		{
			throw "Wrong destination address";
		}
	}
	
	else if(incomingMessage->messageType == "ack")
	{
		if(kernelMap[nodeAddress] == 0)
		{
			MasterKernelReceivedAck();
			if(startNextPhase)
			{
				//std::cout<<"@@@@@@@###### sending a bunch of ACKS"<<endl;
				returnMessage = SendAckMessage(--ackCount);
				ackCount--;
			}
		}
		else 
		{
			waitForAck = false;
			
			currentPhase++;
			returnMessage = MessageCreated();
			
		}
		
	}
	else if(incomingMessage->messageType == "message_ack")
	{
		waitingForMessageAck = false;
		returnMessage = MessageCreated();
	}
	else 
	{
		throw "lbltrace_ackall_trafficgenerator.cpp: Unknown message type.";
	}

	
	delete incomingMessage;
	
	return returnMessage;
}

ElectronicMessage* LBLTrace_AckAll_TrafficGenerator::MessageCreated()
{
	int nextMessageDestinationKernel, nextMessageSize;
	
	if(startNextPhase)
	{
		//std::cout<<"sending a bunch of ACKS"<<endl;
		if(ackCount != 0)
		{
			return SendAckMessage(ackCount--);
		}
		else 
		{
			startNextPhase = false;
			waitForAck = false;
			currentPhase++;
			return MessageCreated();
		}
		
		
	}
	
	if(!messagePhaseQueue[nodeKernelId].empty() && currentPhase == messagePhaseQueue[nodeKernelId].front() && !waitingForMessageAck)
	{
		nextMessageDestinationKernel = messageDestinationKernelQueue[nodeKernelId].front();
		nextMessageSize = messageSizeQueue[nodeKernelId].front();
		
		messagePhaseQueue[nodeKernelId].pop();
		messageDestinationKernelQueue[nodeKernelId].pop();
		messageSizeQueue[nodeKernelId].pop();
		
		waitingForMessageAck = true;
		
		return SendDataMessage(nextMessageDestinationKernel, nextMessageSize);
	}
	else 
	{
		if(!waitForAck && !waitingForMessageAck)
		{
			waitForAck = true;
			
			if(kernelMap[nodeAddress] == 0)
			{
				//std::cout<<nodeAddress<<" "<<nodeKernelId<<" !!!!!!!!!!! self ack"<<endl;
				MasterKernelReceivedAck();
				
				if(startNextPhase)
				{
					//std::cout<<"@@@@@@@###### sending a bunch of ACKS"<<endl;
					ackCount--;
					return SendAckMessage(ackCount--);
				}
				else 
				{
					return NULL;
				}

			}
			else 
			{
				// Let kernel 0 be the master kernel
				return SendAckMessage(0);
			}
		}
		else 
		{
			return NULL;
		}
		
		
	}
	
}


ElectronicMessage* LBLTrace_AckAll_TrafficGenerator::SendDataMessage(int destinationKernelId, int messageSize)
{
	ElectronicMessage* msg = new ElectronicMessage;
	msg->messageId = GetUniqueId();
	msg->messageType = "data";
	msg->sourceAddress = nodeAddress;
	msg->destinationAddress = processorMap[destinationKernelId];
	msg->messageSize = messageSize;
	
	//std::cout<<"sending "<<nodeAddress<<" --> "<<msg->destinationAddress<<endl;
	return msg;
}

ElectronicMessage* LBLTrace_AckAll_TrafficGenerator::SendAckMessage(int kernelId)
{
	ElectronicMessage* msg = new ElectronicMessage;
	msg->messageId = GetUniqueId();
	msg->messageType = "ack";
	msg->sourceAddress = nodeAddress;
	msg->destinationAddress = processorMap[kernelId];
	msg->messageSize = 32; // arbitrary size
	
	return msg;
}

ElectronicMessage* LBLTrace_AckAll_TrafficGenerator::SendMessageAckMessage(Address destinationAddress)
{
	ElectronicMessage* msg = new ElectronicMessage;
	msg->messageId = GetUniqueId();
	msg->messageType = "message_ack";
	msg->sourceAddress = nodeAddress;
	msg->destinationAddress = destinationAddress;
	msg->messageSize = 32; // arbitrary size
	
	return msg;
}

void LBLTrace_AckAll_TrafficGenerator::MasterKernelReceivedAck()
{
	ackCount++;
	//std::cout<<"ack count:"<<ackCount<<endl;
	if(ackCount == networkSize && currentPhase != lastPhase)
	{
		//std::cout<<"@@@@@@ Next Phase:"<<currentPhase + 1<<endl;
		startNextPhase = true;
	}
}
