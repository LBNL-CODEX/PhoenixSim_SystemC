/*
 *  configuration.h
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/18/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

#include <string>
#include <vector>
#include <map>
#include <set>
#include <vector>
#include <typeinfo>

namespace PhoenixSim
{
	class Configuration;
	
	enum ConfigType {CONFIG_NONE, CONFIG_INT, CONFIG_STRING, CONFIG_DOUBLE};
	
	class AbstractConfigValue
	{
		public:
			const ConfigType type;
			int numOfValues;
		
			std::map<std::string, int> runMultipler;
		
			AbstractConfigValue(ConfigType p_type)
			:type(p_type), numOfValues(0)
			{
			}
		
			virtual int GetSize() = 0;
	};
	
	class ConfigString: public AbstractConfigValue
	{
	public:
		ConfigString()
		:AbstractConfigValue(CONFIG_STRING)
		{
		}
		std::vector<std::string> data;
		int GetSize() 
		{
			return data.size();
		}
	};
	
	class ConfigDouble: public AbstractConfigValue
	{
	public:
		ConfigDouble()
		:AbstractConfigValue(CONFIG_DOUBLE)
		{
		}
		std::vector<double> data;
		int GetSize() 
		{
			return data.size();
		}
	};

	std::ostream& operator <<(std::ostream& os, Configuration const &v);
	
	class Configuration
	{
		public:
			Configuration();
			~Configuration();
		
		
		
		
			bool ParseFile(std::string fileName);
			bool ParseLine(std::string configLine);
		
		private:
			bool IsBlank(std::string line);
			bool IsSectionDeclaration(std::string line);
			bool IsValueAssignment(std::string line);
			std::vector<std::string> GetSectionFields(std::string line);

			std::pair< std::vector<std::string>, AbstractConfigValue*> GetAssignmentArguments(std::string line);
			std::vector<std::string> ParseName(std::string fullName);
			AbstractConfigValue* ParseValue(std::string fullValue);
			std::string ParseString(std::string value);
			double ParseDouble(std::string value);
			AbstractConfigValue* ParseArray(std::string fullValue);
		
			std::string currentSection;
		
			// Top Level Map: String key indicating General or Run parameters
			//					Value is a set of maps
			// 2nd Level Set: A set of maps that have identifiers and values
			// 3rd Level Map: Key is a vector identifier, and value is the argument for the parameter
			// 4th Level Vector: the full path of the parameter name, elements indicate words delimited by a period (.)
			std::map< std::string, std::map< std::vector<std::string>, AbstractConfigValue* > > parameters;
		
			
			int NumOfRuns(std::string config = "General");
		
			friend std::ostream& operator <<(std::ostream& os, const Configuration &v);	
	};
	
}


#endif // __CONFIGURATION_H__

