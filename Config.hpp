#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

#include "PluginsAPI.hpp"

/***********************************************************************************/
// Config class 
// Class that used for reading config file and getting arguments to plugins from it.
// 
// Input: fileName :=> std::string
// Output: array of proccesed arguments :=> std::vector<COnverterArguments>
/***********************************************************************************/

class Config
{
public:
	Config(std::string configFile);

	~Config();

	std::vector<CVTARGS>& getArgs(std::map<std::string, BaseConverter*>& _registeredPlugins);

private:
	std::string _configFile;
	std::vector<CVTARGS> _converterArgs;
	CVTARGS a;

	void parseConfigFile(std::map<std::string, BaseConverter*>& _registeredPlugins);
};


