#include "MuteConverter.h"
#include <string>

//Plugin working logic
mediaStream* MuteConverter::proccesWav(CVTARGS& args)
{
	/* Arguments checking */
	if (args.numOfAdditionalStream != 0) throw BadConfigException("Too many arguments. This plugin doesn't support additional streams!");

	mediaStream* mainStream = args.mainStream.stream;

	int begin	= std::stol(args.args[1]);
	int end		= std::stol(args.args[2]);

	begin *= sampleRate; end *= sampleRate;

	if (begin > args.mainStream.size || end > args.mainStream.size) throw BadConfigException("Arguments [time] is out of wav file length.");

	while (begin != end)
	{
		mainStream[begin] = 0;
		begin++;
	}
	
	return mainStream;
}

//Get plugin's reference
void MuteConverter::getReference()
{
	std::cout << "Mute converter." << std::endl << 
		"Arguments: [start point] , [end point] - start and end position." << 
		std::endl << "Output: [stream] - proccesed file." << std::endl;
}

//Register plugin in program
std::string MuteConverter::setMyName()
{
	return this->name;
}

//Fabric function that creates object
BaseConverter* _cdecl createMe() {
	return new MuteConverter();
}
