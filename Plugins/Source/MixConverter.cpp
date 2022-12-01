#include "MixConverter.hpp"

//Plugin working logic
mediaStream* MixConverter::proccesWav(CVTARGS& args)
{
	int begin = std::stoi(args.args[2]) * sampleRate;
	_mediaStream* additionalStream = args.additionalStream[0];

	if (begin >= args.mainStream.size || begin < 0) 
		throw BadConfigException("Argument [time] is out of wav file length.");

	for (size_t i = begin; i < args.mainStream.size; i++)
	{
		if (i == additionalStream->size) break;
		args.mainStream.stream[i] = (args.mainStream.stream[i] + additionalStream->stream[i]) / 2;
	}

	return args.mainStream.stream;
}

//Get plugin's reference
void MixConverter::getReference()
{
	std::cout << "Mix converter." << std::endl << "Mixing two streams." << std::endl <<
		"Arguments : [start point] - start position Output : [stream] - proccesed file." << std::endl;
}

//Register plugin in program
std::string MixConverter::setMyName()
{
	return this->name;
}

//Fabric function that creates object
BaseConverter* _cdecl createMe() {
	return new MixConverter();
}
