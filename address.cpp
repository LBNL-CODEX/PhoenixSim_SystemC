/*
 *  address.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/6/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "address.h"

#include <assert.h>

using namespace std;
using namespace PhoenixSim;


Address::Address()
{
}

Address::~Address()
{
}

int Address::GetAddress(int addressPos) const
{
	return addressComponents[addressPos];
}

void Address::SetAddress(int addressPos, int addressValue)
{
	assert(addressPos < addressComponents.size());
	
	addressComponents[addressPos] = addressValue;
}

bool Address::operator ==(const Address & rhs) const
{
	if(addressComponents.size() != rhs.addressComponents.size())
	{
		return false;
	}
	
	for(int i = 0; i < addressComponents.size(); i++)
	{
		if(addressComponents[i] != rhs.addressComponents[i])
		{
			return false;
		}
	}
	return true;
}

bool Address::operator <(const Address & rhs) const
{
	for(int i = 0 ; i < min(this->GetAddressLength(), rhs.GetAddressLength()); i++)
	{
		if(this->GetAddress(i) != rhs.GetAddress(i))
		{
			return this->GetAddress(i) < rhs.GetAddress(i);
		}
	}
	return this->GetAddressLength() < rhs.GetAddressLength();
}

bool Address::operator !=(const Address & rhs) const
{
	return !((*this) == rhs);
}

Address& Address::operator =(const Address& rhs)
{
	addressComponents.resize(rhs.addressComponents.size());
	for(int i = 0; i < addressComponents.size(); i++)
	{
		addressComponents[i] = rhs.addressComponents[i];
	}
	
	return *this;
}

void Address::SetAddressLength(int length)
{
	addressComponents.resize(length);
}

std::ostream& PhoenixSim::operator <<(std::ostream& os, Address const &addr)
{
	for(int i = 0; i < addr.addressComponents.size(); i++)
	{
		if(i != 0)
		{
			os << ".";
		}
		os << addr.GetAddress(i);
	}
	
	return os;
}

int Address::GetAddressLength() const
{
	return addressComponents.size();
}