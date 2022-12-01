#pragma once

#include <string>
#include <PluginsAPI.hpp>

class MixConverter : public BaseConverter
{
public:
	MixConverter() = default;
	~MixConverter() = default;

	void		getReference();
	std::string setMyName();
	mediaStream*proccesWav(CVTARGS& args);
	std::string name = "mix";
};

extern "C"
{
	DLL_EXPORT BaseConverter* _cdecl createMe();
};
