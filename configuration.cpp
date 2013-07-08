/*
 *  configuration.cpp
 *  PhoenixSim
 *
 *  Created by Johnnie Chan on 7/18/11.
 *  Copyright 2011 Johnnie Chan. All rights reserved.
 *
 */

#include "configuration.h"

#include <iostream>
#include <fstream>
#include <sstream>

using  namespace std;
using namespace PhoenixSim;


Configuration::Configuration()
:currentSection("none")
{

}

Configuration::~Configuration()
{
	
}

bool Configuration::ParseFile(string fileName)
{
	ifstream iniFile;
	
	iniFile.open(fileName.c_str(), ios::in);
	int lineNumber = 0;
	string lineToParse;
	
	while(!iniFile.eof())
	{
		getline(iniFile, lineToParse);
		lineNumber++;
		//std::cout<<lineNumber<<": ";
		//std::cout<<lineToParse<<endl;
		
		ParseLine(lineToParse.substr(0, lineToParse.find('#')));
	}
	
	iniFile.close();
	
	std::cout<<(*this)<<endl;
	std::cout<<"# of runs: "<<NumOfRuns()<<endl;
	
	return true;
}

bool Configuration::ParseLine(std::string configLine)
{
	vector<string> strings;
	
	if(!IsBlank(configLine))
	{
		if(IsSectionDeclaration(configLine))
		{
			strings = GetSectionFields(configLine);
			
			if(strings.empty())
			{
			}
			else if(strings[0] == "General")
			{
				currentSection = "General";
			}
		}
		else if(IsValueAssignment(configLine))
		{
			if(currentSection == "none")
			{
				throw "No section assigned yet";
			}
			else if(currentSection == "General")
			{
				pair<vector<string>, AbstractConfigValue*> args = GetAssignmentArguments(configLine);
				args.second->runMultipler["General"] = 0;
				parameters["General"].insert(GetAssignmentArguments(configLine));
			}
		}
		else 
		{
			throw "Unable to determine line syntax";
		}

	}
	else 
	{
	}
	return true;

}


bool Configuration::IsBlank(std::string line)
{
	size_t firstCharPos = line.find_first_not_of(" \t\n\r");
	return firstCharPos == string::npos;
}

bool Configuration::IsSectionDeclaration(std::string line)
{
	size_t firstCharPos = line.find_first_not_of(" \t\n\r");
	return firstCharPos != string::npos && '[' == line[firstCharPos];
}

vector<string> Configuration::GetSectionFields(std::string line)
{
	vector<string> fields;
	size_t begin, next, end;
	begin = line.find('[');
	end = line.find(']');
	
	begin = line.find_first_not_of(" \t", begin + 1);
	
	while(begin < end)
	{
		next = line.find_first_of(" \t]", begin);
		fields.push_back(line.substr(begin, next - begin));
		begin = next + 1;
	}
	
	
	return fields;
}

bool Configuration::IsValueAssignment(std::string line)
{
	return line.find('=') != string::npos && line.find('=') == line.rfind('=');
}


pair< std::vector<std::string>, AbstractConfigValue*> Configuration::GetAssignmentArguments(std::string line)
{
	pair< std::vector<std::string>, AbstractConfigValue*> returnPair;
	returnPair.first = ParseName(line.substr(0,line.find('=')));
	returnPair.second = ParseValue(line.substr(line.find('=') + 1, string::npos));
	return returnPair;
}

vector<string> Configuration::ParseName(std::string fullName)
{
	vector<string> names;
	
	size_t begin, next, end;
	begin = fullName.find_first_not_of(" \t");
	end = fullName.find_last_not_of(" \t");
	
	while(begin < end)
	{
		next = fullName.find('.', begin);
		if(next > end)
		{
			next = end + 1;
		}
		names.push_back(fullName.substr(begin, next - begin));
		
		if(names.back().empty())
		{
			throw "blank name not allowed";
		}
		
		begin = next + 1;
	}
	
	
	return names;
}

AbstractConfigValue* Configuration::ParseValue(string fullValue)
{
	AbstractConfigValue* configVal = NULL;
	size_t begin, next, end;
	
	begin = fullValue.find_first_not_of(" \t");
	end = fullValue.find_last_not_of(" \t");
	next = fullValue.find_first_of("+-1234567890.$\"");
	if(next == begin)
	{
		if(fullValue[begin] == '"')
		{
			configVal = new ConfigString;
			((ConfigString*)configVal)->data.push_back(ParseString(fullValue));
		}
		else if(fullValue[begin] == '$')
		{
			configVal = ParseArray(fullValue);
		}
		else // probably a number 
		{
			configVal = new ConfigDouble;
			((ConfigDouble*)configVal)->data.push_back(ParseDouble(fullValue));
		}

	}
	else 
	{
		throw "error parsing value";
	}

	return configVal;
}


string Configuration::ParseString(string value)
{
	size_t begin, next;
	
	begin = value.find('"') + 1;
	next = value.find('"', begin);
	return value.substr(begin, next - begin);
}

double Configuration::ParseDouble(string value)
{
	return atof(value.c_str());
}


AbstractConfigValue* Configuration::ParseArray(string fullValue)
{
	size_t begin, next, end;
	
	ConfigType arrayType = CONFIG_NONE;
	AbstractConfigValue *array = NULL;
	
	begin = fullValue.find_first_not_of(" \t");
	end = fullValue.find_last_not_of(" \t");
	next = fullValue.find('$');
	
	
	
	string name;
	
	if(begin != next)
	{
		throw "improper declaration of array";
	}
	
	
	begin = next + 1;
	
	next = fullValue.find('{');
	
	name = fullValue.substr(begin, next - begin);
	
	begin = fullValue.find_first_not_of(" \t", next + 1);
	
	
	if(end != fullValue.find('}', begin))
	{
		throw "incorrect array termination";
	}
	
	std::cout<<"loop:"<<endl;
	
	while(begin < end)
	{
		std::cout<<"begin:"<<begin<<" ";
		next = fullValue.find_first_of(",}", begin);
		
		std::cout<<"next:"<<next<<" ";
		if(fullValue[begin] == '"')
		{
			if(arrayType == CONFIG_NONE)
			{
				arrayType = CONFIG_STRING;
				array = new ConfigString;
			}
			if(arrayType == CONFIG_STRING)
			{
				((ConfigString*)array)->data.push_back(ParseString(fullValue.substr(begin, next - begin)));
			}
			else 
			{
				throw "varying type in array";
			}

		}
		else if(begin == fullValue.find_first_of("+-1234567890.", begin))
		{
			if(arrayType == CONFIG_NONE)
			{
				arrayType = CONFIG_DOUBLE;
				array = new ConfigDouble;
			}
			if(arrayType == CONFIG_DOUBLE)
			{
				((ConfigDouble*)array)->data.push_back(ParseDouble(fullValue.substr(begin, next - begin)));
			}
			else 
			{
				throw "varying type in array";
			}
		}
		else {
			throw "incorrect format for array element";
		}

		
		
		begin = fullValue.find_first_not_of(" \t", next + 1);
	}
	return array;
	
}

ostream& PhoenixSim::operator<<(ostream& os, const Configuration &v)
{
	os<<"Parameters List"<<endl;
	for(map<string, map<vector<string>, AbstractConfigValue* > >::const_iterator it = v.parameters.begin(); it != v.parameters.end(); it++)
	{
		os<<"\t"<<it->first<<endl;
		for(map<vector<string>, AbstractConfigValue* >::const_iterator sub_it = it->second.begin(); sub_it != it->second.end(); sub_it++)
		{
			os<<"\t\t";
			for(int i = 0; i < sub_it->first.size(); i++)
			{
				os<<sub_it->first[i];
				
				if(i + 1 == sub_it->first.size())
				{
					os<<"=";
				}
				else 
				{
					os<<".";
				}

			}
			
			
			if(sub_it->second != NULL)
			{
				os<<"${";
			
				switch(sub_it->second->type)
				{
					case CONFIG_DOUBLE:
						for(int i = 0; i < sub_it->second->GetSize(); i++)
						{
							os<<static_cast<ConfigDouble*>(sub_it->second)->data[i];
							if(i + 1 != sub_it->second->GetSize())
							{
								os<<",";
							}
						}
						break;
					case CONFIG_STRING:
						for(int i = 0; i < sub_it->second->GetSize(); i++)
						{
							os<<static_cast<ConfigString*>(sub_it->second)->data[i];
							if(i + 1 != sub_it->second->GetSize())
							{
								os<<",";
							}
						}
						break;
				}
				os<<"}";
			}
			os<<"\n";
		}
	}
	
	return os;
}



int Configuration::NumOfRuns(string config)
{
	int count = 1;
	
	for(map<vector<string>, AbstractConfigValue* >::const_iterator it = parameters[config].begin(); it != parameters[config].end(); it++)
	{
		count *= it->second->GetSize();
	}
	return count;
}
