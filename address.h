/*
 *  address.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/6/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __ADDRESS_H__
#define __ADDRESS_H__

#include <vector>
#include <ostream>

namespace PhoenixSim
{
	
	class Address
	{
		public:
			Address();
			~Address();
			void SetAddress(int addressPos, int addressValue);
			int GetAddress(int addressPos) const;
			void SetAddressLength(int length);
			int GetAddressLength() const;
		
			bool operator ==(const Address & rhs) const;
			bool operator <(const Address & rhs) const;
			bool operator !=(const Address & rhs) const;
			Address& operator =(const Address& rhs);
			friend std::ostream& operator <<(std::ostream& os, Address const &addr);	
		private:
			std::vector<int> addressComponents;
	};
	
	std::ostream& operator <<(std::ostream& os, Address const &addr);
}


#endif // __ADDRESS_H__

