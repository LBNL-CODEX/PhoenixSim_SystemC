/*
 *  lbltrace_trafficgenerator.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/15/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "lbltrace_trafficgenerator.h"

#include <vector>

#include "electronicmessage.h"

using namespace std;
using namespace PhoenixSim;

map<Address, int> LBLTrace_TrafficGenerator::kernelMap;
map<int, Address> LBLTrace_TrafficGenerator::processorMap;
bool LBLTrace_TrafficGenerator::isInitialized = false;


boost::mt19937 LBLTrace_TrafficGenerator::gen;

class PhaseId_LBLTrace : public Payload
{
	public:
		int phaseId;
};


LBLTrace_TrafficGeneratorParameters::LBLTrace_TrafficGeneratorParameters()
{
}

LBLTrace_TrafficGenerator::LBLTrace_TrafficGenerator()
:ackCount(0), waitForAck(false), traceDone(false), startNextPhase(false)
{	
}

LBLTrace_TrafficGenerator::~LBLTrace_TrafficGenerator()
{
}


void LBLTrace_TrafficGenerator::SetTrafficGeneratorParameters(TrafficGeneratorParameters *p_parameters)
{
	
	LBLTrace_TrafficGeneratorParameters* parameters = static_cast<LBLTrace_TrafficGeneratorParameters*>(p_parameters);
	int nextMessagePhase, nextMessageSourceKernel, nextMessageDestinationKernel, nextMessageSize;
	
	
	addressSpace = parameters->addressSpace;
	networkSize = parameters->networkSize;
	nodeAddress = parameters->nodeAddress;
	traceFile.open(parameters->fileName.c_str(), ios::in);
	assert(!traceFile.fail());
	
	traceFile >> currentPhase;
	traceFile.seekg(ios::beg);
			
	InitializeKernelInterfaceMapping();
	//std::cout<<"I am "<<kernelMap[nodeAddress]<<endl;
	
	
	int currentExaminedPhase = currentPhase;
	int expectedMessageCount = 0;
	
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
			if(nextMessageSourceKernel == kernelMap[nodeAddress])
			{
				//std::cout<<"storing "<<nextMessagePhase<<" "<<nextMessageSourceKernel<<" "<<nextMessageDestinationKernel<<" "<<nextMessageSize<<endl;
				messagePhaseQueue.push(nextMessagePhase);
				messageDestinationKernelQueue.push(nextMessageDestinationKernel);
				messageSizeQueue.push(nextMessageSize*8);
		
			}
			
			
			if(nextMessageDestinationKernel == kernelMap[nodeAddress])
			{
				if(currentExaminedPhase == nextMessagePhase)
				{
					expectedMessageCount++;
				}
				else 
				{
					expectedMessages[currentExaminedPhase] = expectedMessageCount;
					expectedMessageCount = 1;
					
					currentExaminedPhase++;
					
					while(currentExaminedPhase != nextMessagePhase)
					{
						expectedMessages[currentExaminedPhase] = 0;
						currentExaminedPhase++;
					}
				}
			}
			
			lastPhase = nextMessagePhase;
		}
		else 
		{
			expectedMessages[currentExaminedPhase] = expectedMessageCount;
			
			break;
		}

	}while(true);
	
	map<int,int>::iterator it;
	
	
	traceFile.close();
	/*
	std::cout<<nodeAddress<< " "<<kernelMap[nodeAddress]<<endl;
	for(map<int,int>::iterator it = expectedMessages.begin(); it != expectedMessages.end(); it++)
	{
		std::cout<<it->first<<" "<<it->second<<endl;
	}*/
	
	if(kernelMap[nodeAddress] == 0)
	{
		std::cout<<"starting phase: "<<currentPhase<<endl;
		std::cout<<"last phase: "<<lastPhase<<endl;
	}
}

void LBLTrace_TrafficGenerator::InitializeKernelInterfaceMapping()
{
	if(!isInitialized)
	{
		RandomKernelInterfaceMapping();
		isInitialized = true;
	}
	
	
	//std::cout<<"I am processor "<<nodeAddress<<" mapped to kernel "<< kernelMap[nodeAddress] << " which connected to proc: "<<processorMap[kernelMap[nodeAddress]]<<endl;
}

void LBLTrace_TrafficGenerator::RandomKernelInterfaceMapping()
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

ElectronicMessage* LBLTrace_TrafficGenerator::CreateFirstMessage()
{
	return MessageCreated();
}

ElectronicMessage* LBLTrace_TrafficGenerator::MessageReceived(ElectronicMessage* incomingMessage)
{
	// if this node is the master node
	ElectronicMessage *returnMessage = NULL;
	
	PhaseId_LBLTrace* phaseData = static_cast<PhaseId_LBLTrace*> (incomingMessage->data);
	
	//if(kernelMap[incomingMessage->destinationAddress] == 0)
	//std::cout<<nodeAddress<<" "<<kernelMap[nodeAddress]<<" received "<<incomingMessage->messageType<<" from "<<incomingMessage->sourceAddress<<":"<<kernelMap[incomingMessage->sourceAddress]<<endl;
	
	if(incomingMessage->messageType == "data")
	{
		if(expectedMessages.find(phaseData->phaseId) == expectedMessages.end() || expectedMessages[phaseData->phaseId] == 0)
		{
			std::cout<<"current phase:"<<currentPhase<<" message phase:"<<phaseData->phaseId<<endl;
			std::cout<<incomingMessage->sourceAddress<<" "<<kernelMap[incomingMessage->sourceAddress]<<" "<<incomingMessage->destinationAddress<<" "<<kernelMap[incomingMessage->destinationAddress]<<endl;
			std::cout<<"here at "<<nodeAddress<<" for kernel "<<kernelMap[nodeAddress]<<endl;
			throw "Unexpected data message received";
		}
		
		if(incomingMessage->destinationAddress == nodeAddress)
		{
			expectedMessages[phaseData->phaseId]--;
			if(expectedMessages.find(currentPhase) == expectedMessages.end() || expectedMessages[currentPhase] == 0)
			{
				returnMessage = MessageCreated();
			}
		}
		else 
		{
			throw "Wrong destination address";
		}
	}
	
	if(incomingMessage->messageType == "ack")
	{
		if(kernelMap[nodeAddress] == 0)
		{
			MasterKernelReceivedAck();
			if(startNextPhase)
			{
				returnMessage = SendAckMessage(--ackCount);
				ackCount--;
			}
		}
		else 
		{
			waitForAck = false;
			
			//std::cout<<nodeAddress<<":"<<kernelMap[nodeAddress]<<" ("<<expectedMessages[currentPhase]<<") incrementing phase "<<currentPhase<<" --> ";
			currentPhase++;
			//std::cout<<currentPhase<<"("<<expectedMessages[currentPhase]<<")"<<endl;
			returnMessage = MessageCreated();
			
		}

	}
	
	delete incomingMessage;
	
	return returnMessage;
}

ElectronicMessage* LBLTrace_TrafficGenerator::MessageCreated()
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
	
	if(!messagePhaseQueue.empty() && currentPhase == messagePhaseQueue.front())
	{
		nextMessageDestinationKernel = messageDestinationKernelQueue.front();
		nextMessageSize = messageSizeQueue.front();
		
		messagePhaseQueue.pop();
		messageDestinationKernelQueue.pop();
		messageSizeQueue.pop();

		
		return SendDataMessage(nextMessageDestinationKernel, nextMessageSize);
	}
	else 
	{
		if(!waitForAck && (expectedMessages.find(currentPhase) == expectedMessages.end() || expectedMessages[currentPhase] == 0))
		{
			//std::cout<<"nothing left to send, sending an ACK"<<endl;
			waitForAck = true;
			
			if(kernelMap[nodeAddress] == 0)
			{
				//std::cout<<"!!!!!!!!!!! self ack"<<endl;
				MasterKernelReceivedAck();
				if(startNextPhase)
				{
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


ElectronicMessage* LBLTrace_TrafficGenerator::SendDataMessage(int destinationKernelId, int messageSize)
{
	ElectronicMessage* msg = new ElectronicMessage;
	msg->messageId = GetUniqueId();
	msg->messageType = "data";
	msg->sourceAddress = nodeAddress;
	msg->destinationAddress = processorMap[destinationKernelId];
	msg->messageSize = messageSize;
	msg->data = new PhaseId_LBLTrace();
	(((PhaseId_LBLTrace*)(msg->data))->phaseId) = currentPhase;
	
	//std::cout<<"sending "<<nodeAddress<<" --> "<<msg->destinationAddress<<endl;
	return msg;
}

ElectronicMessage* LBLTrace_TrafficGenerator::SendAckMessage(int kernelId)
{
	ElectronicMessage* msg = new ElectronicMessage;
	msg->messageId = GetUniqueId();
	msg->messageType = "ack";
	msg->sourceAddress = nodeAddress;
	msg->destinationAddress = processorMap[kernelId];
	msg->messageSize = 32; // arbitrary size
	
	return msg;
}
	
void LBLTrace_TrafficGenerator::MasterKernelReceivedAck()
{
	ackCount++;
	//std::cout<<"####### recieved so far:"<<ackCount<<endl;
	if(ackCount == networkSize && currentPhase != lastPhase)
	{
		std::cout<<"@@@@@@ Next Phase:"<<currentPhase + 1<<endl;
		startNextPhase = true;
		
	}
}
