#ifndef __ORION_POWER_PARAMS__
#define __ORION_POWER_PARAMS__

namespace PhoenixSim{
	
	//POD so default copy, assignment and destructor are fine

	class COrionPowerParams {
	public:
		COrionPowerParams(){
					m_technology	= 32;
					m_voltage	= 1.0;
					m_frequency	= 1e9;
					m_transType	= NP_GATE;
				   }

		int 	m_technology;
		double 	m_voltage;
		double 	m_frequency;
		int 	m_transType;
	};
}

#endif
