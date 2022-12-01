#pragma once

#include <iostream>
#include <vector>
#include <exception>

#include <PluginsAPI.hpp>

class MuteConverter : public BaseConverter
{
public:
	MuteConverter()	= default;
	~MuteConverter() = default;

	void			getReference();
	std::string		setMyName();
	mediaStream*	proccesWav(CVTARGS& args);
	std::string		name = "mute";
};

extern "C" 
{
	DLL_EXPORT BaseConverter* _cdecl createMe();
};
