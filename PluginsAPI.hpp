#pragma once

#include <iostream>
#include <vector>
#include <exception>

#ifdef _DLL_EXPORT                     
#define DLL_EXPORT __declspec(dllexport)
#else                                  
#define DLL_EXPORT __declspec(dllimport)
#endif

const int byteRate		= 0x5888;
const int sampleRate	= 44100;
const int numOfChannels = 1;

using PluginArgs = std::vector<std::string>;
using mediaStream = short;

struct __mediaStream
{
	mediaStream* stream;
	unsigned long long size;
};

using _mediaStream = __mediaStream;

class CVTARGS
{
public:
	CVTARGS()
	{
		this->mainStream			= { 0 };
		this->additionalStream		= nullptr;
		this->numOfAdditionalStream = 0;
	}

	~CVTARGS() 
	{ 
		if (additionalStream != nullptr)
		{
			for (int i = 0; i < numOfAdditionalStream; i++)
			{
				delete[] additionalStream[i]->stream;
				delete additionalStream[i];
			}

			delete additionalStream;
			additionalStream = nullptr;
		}
	}

	_mediaStream	mainStream;
	_mediaStream**	additionalStream;

	int				numOfAdditionalStream;
	PluginArgs		args;
};

using ConverterArguments = CVTARGS;

class BaseConverter
{
public:
	virtual ~BaseConverter() {};
	virtual mediaStream* proccesWav(CVTARGS& args) = 0;
	virtual void getReference()	= 0;
	virtual std::string setMyName() = 0;

protected:
	mediaStream*	soundStream;
	unsigned int	begin, end;
	int				converterID;
};

typedef BaseConverter* (_cdecl* REG_ME) ();

class BadConfigException : public std::runtime_error
{
public:
	BadConfigException(std::string message) : runtime_error(message.c_str()) {};
};

class UnknownPlugin : public std::runtime_error
{
public:
	UnknownPlugin(std::string message) : runtime_error(message.c_str()) {};
};

class UnsupportedWav : public std::runtime_error
{
public:
	UnsupportedWav(std::string message) : runtime_error(message.c_str()) {};
};