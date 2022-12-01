#pragma once

#include <string>
#include <PluginsAPI.hpp>

class MyConverter : public BaseConverter
{
public:
	MyConverter() = default;
	~MyConverter() = default;

	void		getReference();
	std::string setMyName();
	mediaStream* proccesWav(CVTARGS& args);
	std::string name = "LimitMe";
};

extern "C"
{
	DLL_EXPORT BaseConverter* _cdecl createMe();
};
