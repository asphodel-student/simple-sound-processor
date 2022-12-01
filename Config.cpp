#include "PluginsAPI.hpp"
#include "Config.hpp"

Config::Config(std::string configFile)
{
	this->_configFile = configFile;
}

Config::~Config() = default;

std::vector<CVTARGS>& Config::getArgs(std::map<std::string, BaseConverter*>& _registeredPlugins)
{
	this->parseConfigFile(_registeredPlugins);
	return _converterArgs;
}

void Config::parseConfigFile(std::map<std::string, BaseConverter*>& _registeredPlugins)
{
	std::ifstream fin(_configFile);
	std::string _currentLine;
	std::vector<std::string> parsedLine;

	while (std::getline(fin, _currentLine))
	{
		CVTARGS* newArgument = new CVTARGS();

		size_t start;
		size_t end = 0;

		if (_currentLine.empty() == true || _currentLine.front() == '#') continue;

		while ((start = _currentLine.find_first_not_of(' ', end)) != std::string::npos)
		{
			end = _currentLine.find(' ', start);
			parsedLine.push_back(_currentLine.substr(start, end - start));
		}

		if (_registeredPlugins.find(parsedLine[0]) == _registeredPlugins.end())
			throw UnknownPlugin("Unknown plugin " + parsedLine[0]);

		for (auto& x : parsedLine)
			if (x.front() == '$') newArgument->numOfAdditionalStream++;

		newArgument->mainStream.stream		= nullptr;
		newArgument->args					= parsedLine;
		newArgument->additionalStream		= nullptr;	//TO DO

		this->_converterArgs.push_back(*newArgument);

		parsedLine.clear();
	}
}