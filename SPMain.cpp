#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#include "argparse.hpp"
#include "PluginsAPI.hpp"
#include "Config.hpp"
#include "WavProccess.hpp"

#include <Windows.h>

/************************************************************************/
// Simple sound proccesor
// Arguments:
// -h [optional] - show reference
// -c <file-name.txt> - config file: include all commands
// <files-n.waw>... - media files to be processed with the wav extension
/************************************************************************/

void searchAndOpenWavStreams(ConverterArguments& args)
{

	if (args.numOfAdditionalStream == 0) return;

	int i = 0; 		
	args.additionalStream = new _mediaStream*;

	for (auto x : args.args)
	{
		if (!x.empty() && x.front() == '$')
		{
			x.erase(x.begin());
			Wav __temp(x);

			args.additionalStream[i]			= new _mediaStream;
			args.additionalStream[i]->stream	= new mediaStream[__temp._wav_data.wav_header.subchunk2Size];
			args.additionalStream[i]->size	= __temp._wav_data.wav_header.subchunk2Size;
			//args.numOfAdditionalStream++;
			memcpy(args.additionalStream[i++]->stream, __temp.getStream(), __temp._wav_data.wav_header.subchunk2Size);
		}
	}
}


void showReference(std::map<std::string, BaseConverter*>& _registeredPlugins)
{
	for (auto& x : _registeredPlugins)
	{
		x.second->getReference();
		std::cout << std::endl;
	}
}

//All registered plugins
using Plugins = std::map<std::string, BaseConverter*>;
std::map<std::string, BaseConverter*> registeredPlugins;

int main(int argc, char** argv)
{
	std::cout << "\t\t\t\t\tSound proccesor" << std::endl;

	//Parsing arguments
	argparse::ArgumentParser sound_proccesor("sound_proccesor");
	sound_proccesor.add_argument("-c").help("Config file").nargs(1);
	sound_proccesor.add_argument("-h").default_value(false).implicit_value(true);
	sound_proccesor.add_argument("files").remaining();

	try 
	{
		sound_proccesor.parse_args(argc, argv);
	} 
	catch (const std::runtime_error& err) 
	{
		std::cerr << err.what() << std::endl;
		std::cerr << sound_proccesor;
		std::exit(1);
	}

	std::vector<std::string> files;
	try 
	{
		files = sound_proccesor.get<std::vector<std::string>>("files");
	} 
	catch (std::logic_error& e) 
	{
		std::cout << "No files provided" << std::endl;
	}

	std::string inputConfig;
	try 
	{
		inputConfig = sound_proccesor.get<std::string>("-c");
	} 
	catch (std::logic_error& e) 
	{
		std::cout << "No config file provided" << std::endl;
	} 

	auto help = sound_proccesor.get<bool>("-h");

	std::cout << "Config file: " << inputConfig << std::endl << std::endl;

	//Scan and load plugins
	WIN32_FIND_DATAW wfd;
	HANDLE const hFind = FindFirstFileW(L"./Plugins/*", &wfd);

	std::vector<std::wstring> allPlugins;
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			allPlugins.push_back(wfd.cFileName);
		} while (FindNextFileW(hFind, &wfd) != NULL);

		//Remove useless directories
		allPlugins.erase(allPlugins.begin());
		allPlugins.erase(allPlugins.begin());

		FindClose(hFind);
	}

	//Register all found plugins
	for (auto plugin : allPlugins)
	{
		HINSTANCE hDLL = LoadLibrary(plugin.c_str());
		if (hDLL != 0)
		{
			REG_ME pEntryFunction = (REG_ME)GetProcAddress(hDLL, "createMe");
			BaseConverter* temp = pEntryFunction();
			registeredPlugins.insert({ temp->setMyName(), temp });
		}
		else std::cout << "Plugin " << plugin.c_str() << " is not loaded" << std::endl;
	}

	//Show reference
	if (help)
	{
		std::cout << "REFERENCE:" << std::endl << std::endl;
		showReference(registeredPlugins);
	}

	std::cout << "LOG:" << std::endl << std::endl;

	Wav currentWav(files[0]);

	//Load and read config file
	Config config(inputConfig);
	std::vector<CVTARGS> args;

	try
	{
		args = config.getArgs(registeredPlugins);

	}
	catch (const std::runtime_error& e)
	{
		std::cout << std::endl << e.what() << std::endl << "All supported plugins:" << std::endl << std::endl;
		showReference(registeredPlugins);
		exit(0);
	}
	
	//Do work
	static const int pluginNameArg = 0;
	for (auto &arg : args)
	{
		//Set up arguments
		arg.mainStream.stream	= currentWav._wav_data.data;
		arg.mainStream.size		= currentWav._wav_data.wav_header.subchunk2Size;
		arg.additionalStream	= nullptr;

		searchAndOpenWavStreams(arg);

		//Procces audio stream
		std::cout << arg.args[pluginNameArg] << " is working" << std::endl;
		try 
		{
			currentWav._wav_data.data = registeredPlugins[arg.args[pluginNameArg]]->proccesWav(arg);
		}
		catch (BadConfigException& e)
		{
			std::cout << e.what() << std::endl;
			registeredPlugins[arg.args[pluginNameArg]]->getReference();
			exit(-1);
		}
		std::cout << arg.args[pluginNameArg] << " is done" << std::endl << std::endl;
	}

	//Save proccesed file
	std::ofstream fout("test___.wav", std::ios_base::binary);

	unsigned long long int size = sizeof(WavHeader);
	fout.write(reinterpret_cast<char*>(&currentWav._wav_data.wav_header), sizeof(WavHeader));
	fout.write(reinterpret_cast<char*>(currentWav._wav_data.data), currentWav._wav_data.wav_header.subchunk2Size);

	//Remove plugins
	for (auto x : registeredPlugins)
	{
		delete x.second;
	}

	return 0;
}