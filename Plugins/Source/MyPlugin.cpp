#include <iostream>

#include <PluginsAPI.hpp>
#include "MyPlugin.hpp"

void MyConverter::getReference()
{
	std::cout << "LimitMe converter." << std::endl << "Limits maximum and minimun value of amplitude." << std::endl <<
		"Arguments : [start point, end point] - time positions. Output : [stream] - proccesed file." << std::endl;
}

std::string MyConverter::setMyName()
{
	return this->name;
}

mediaStream* MyConverter::proccesWav(CVTARGS& args)
{
	int begin		= std::stoi(args.args[1]) * sampleRate;
	int end			= std::stoi(args.args[2]) * sampleRate;
	short trashold	= std::stoi(args.args[3]);

	if (begin >= args.mainStream.size || begin < 0)
		throw BadConfigException("Argument [time] is out of wav file length.");

	for (size_t i = begin; i < end; i++)
	{
		if (args.mainStream.stream[i] > trashold || args.mainStream.stream[i] < -trashold) args.mainStream.stream[i] = trashold;
	}

	return args.mainStream.stream;
}

//Fabric function that creates object
BaseConverter* _cdecl createMe() {
	return new MyConverter();
}