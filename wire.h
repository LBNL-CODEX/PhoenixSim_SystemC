/*
 *  wire.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/5/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 *	The purpose of this Wire module is to model the additional delay required 
 *	when buffering is needed to support a clock rate that is faster than that 
 *	which is required to propagate down the metal wire.
 *
 *	p_clockPeriod - the clock period at which the network devices are operating
 *
 *	p_wireCycleDelay - the delay of the wire, in terms of number of buffering 
 *						stages required (i.e. clock cycles). The actual wire 
 *						delay will be equated as 
 *						p_clockPeriod * p_wireCycleDelay. If p_wireCycleDelay
 *						is set to 0, then this implements the behavior of no
 *						buffering and mimics the behavior of simply having an
 *						sc_buffer channel.
 */

#ifndef __WIRE_H__
#define __WIRE_H__

#include <list>

#include "systemc.h"
#include "electronicmessage.h"

#include "orion/ORION_Link.h"
#include "OrionPowerParams.h"

namespace PhoenixSim 
{
	template <typename T>
	SC_MODULE(Wire)
	{		
		sc_in<T>* inputPort;
		sc_out<T>* outputPort;
		
		SC_HAS_PROCESS(Wire);
		
		ORION_Link* power_;
		double ePerBit_;
		double energy_;
		double m_staticPower;


		Wire(sc_module_name name, sc_time p_clockPeriod, unsigned int p_wireCycleDelay, const COrionPowerParams& powerParams )
		:sc_module(name), wireDelay(p_clockPeriod * p_wireCycleDelay)
		{
			inputPort = new sc_in<T>("port_input");
			outputPort = new sc_out<T>("port_output");

			//length of wire (in microns), i just made this up
			double l = 1000;

			int technology = powerParams.m_technology;
			int trans_type = powerParams.m_transType;
			double voltage = powerParams.m_voltage;
			double frequency = powerParams.m_frequency;

			ORION_Tech_Config *conf = new ORION_Tech_Config(technology, trans_type,
					voltage, frequency);
			ePerBit_ = ORION_Link::LinkDynamicEnergyPerBitPerMeter(l * 1e-6,
					conf);

			m_staticPower
			= ORION_Link::LinkLeakagePowerPerMeter(l * 1e-6, conf);

			ePerBit_ *= (l * 1e-6);
			m_staticPower *= (l * 1e-6);

#ifdef __REPORT_STATIC_POWER_AT_STARTUP__
			std::cout << "Reporting WIRE static POWER: " << m_staticPower << " W \n";
#endif
			energy_ = 0;


			// Orion can figure out optimal buffering if you want
			//int del;
			//double h;
			//ORION_Link::getOptBuffering(&del, &h, l * 1e-6, conf);
			//propDelay = del * clockPeriod;
			
			SC_METHOD(TraverseWire);
			dont_initialize();
			sensitive << dataEvent;
			SC_METHOD(DataAtInput);
			dont_initialize();
			sensitive << inputPort->value_changed();
			
		}

		double	GetCumulativePower(){
			return (energy_ + m_staticPower);
		};
		
		~Wire()
		{
#ifdef __REPORT_TOTAL_ENERGY_AT_TEARDOWN__
			std::cout << "Reporting final ENERGY for WIRE: " << energy_ + m_staticPower << " J \n";
#endif

			delete inputPort;
			delete outputPort;
		}
		
	private:
		std::list<T> dataOnWire;
		std::list<sc_time> timeToLive;
		
		sc_time wireDelay;
		
		sc_event dataEvent;
		
		void TraverseWire()
		{
			outputPort->write(dataOnWire.front());
			dataOnWire.pop_front();
			timeToLive.pop_front();
			
			// --------- PUT energy consumption here -------- //
			//energy_ += ePerBit_ * (messageSize);
			

			if(dataOnWire.size() != 0)
			{
				dataEvent.notify(timeToLive.front());
			}
		}
		
		void DataAtInput()
		{			


			dataOnWire.push_back(inputPort->read());
			timeToLive.push_back(wireDelay);
			if(dataOnWire.size() == 1)
			{
				dataEvent.notify(timeToLive.front());
			}
		}
	};
}

#endif // __WIRE_H__

