#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

using mediaStream = short;

#pragma pack(push, 1)
typedef struct Ftm_s
{
	int		subchunk1Id;
	int		subchunk1Size;
	short	audioFormat;
	short	numChannels;
	int		sampleRate;
	int		byteRate;
	short	blockAlign;
	short	bitsPerSample;
} Ftm;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct WavHeader_s
{
	char	chunkId[4];
	int		chunkSize;
	int		format;
	Ftm		ftm;
	int		subchunk2Id;
	int		subchunk2Size;
} WavHeader;
#pragma pack(pop)

typedef struct chunk_s
{
	int id;
	int size;
} chunk;

#pragma pack(push, 1)
typedef struct _Wav_s
{
	WavHeader wav_header;
	mediaStream* data;
} _Wav;
#pragma pack(pop)

class Wav
{
public:
	Wav(std::string fileName) { this->fileName = fileName; readWavFile(fileName); };

	~Wav() { delete[] this->_wav_data.data; };

	unsigned long long int
		readWavFile(std::string fileName)
	{
		const unsigned long long int _data = 1635017060;

		std::ifstream fin(fileName, std::ios_base::binary);
		if (!fin.is_open()) return 0;

		int fileSize = 0;
		fin.seekg(0, std::ios::end);
		fileSize = fin.tellg();
		fin.seekg(0);

		fin.read(reinterpret_cast<char*>(&_wav_data.wav_header), sizeof(WavHeader));

		try {
			this->_isCorrectWav();
		}
		catch (const std::runtime_error& e) {
			std::cout << e.what() << std::endl;
		}

		std::cout << "File " << fileName << " is opened" << std::endl;

		int _trashSize = 0; chunk _tempChunk;
		if (_wav_data.wav_header.subchunk2Id != _data)
		{
			while (!fin.eof())
			{
				fin.read(reinterpret_cast<char*>(&_tempChunk), sizeof(chunk));

				if (_tempChunk.id == _data)
				{
					_wav_data.wav_header.subchunk2Id = _tempChunk.id;
					_wav_data.wav_header.subchunk2Size = _tempChunk.size;
					break;
				}

				if (_tempChunk.size == _data)
				{
					fin.read(reinterpret_cast<char*>(&_wav_data.wav_header.subchunk2Size), sizeof(int));
					_wav_data.wav_header.subchunk2Id = _tempChunk.size;
					break;
				}
				_trashSize += sizeof(chunk);
			}
		}

		std::cout << "Opened file parameters:" << std::endl;
		std::cout << "Bits per sample: " << _wav_data.wav_header.ftm.bitsPerSample << std::endl;
		std::cout << "Byte rate: " << _wav_data.wav_header.ftm.byteRate << std::endl;
		std::cout << "Num of channels: " << _wav_data.wav_header.ftm.numChannels << std::endl;
		std::cout << "Sample rate: " << _wav_data.wav_header.ftm.sampleRate << std::endl;


		unsigned long long int dataSize = 1LL * fileSize - sizeof(WavHeader) - _trashSize;
		_wav_data.wav_header.subchunk2Size = fileSize;

		_wav_data.data = new short[dataSize];

		fin.read(reinterpret_cast<char*>(_wav_data.data), 1LL * dataSize);

		std::ofstream fout("__" + fileName, std::ios_base::binary);

		fout.write(reinterpret_cast<char*>(&_wav_data.wav_header), sizeof(WavHeader));
		fout.write(reinterpret_cast<char*>(_wav_data.data), dataSize);

		fin.close();
		fout.close();

		return dataSize;
	}

	mediaStream* getStream() { return _wav_data.data; }
	std::string fileName;

	_Wav _wav_data;
private:

	bool _isCorrectWav()
	{
		std::string _temp_chunk(_wav_data.wav_header.chunkId), _temp_to_cmp = "RIFF";
		_temp_chunk.erase(4);
		if (_temp_chunk != _temp_to_cmp)
		{
			throw std::runtime_error("Not wav format");
		}

		if ((_wav_data.wav_header.ftm.numChannels != 1) || (_wav_data.wav_header.ftm.sampleRate != 44100) ||
			(_wav_data.wav_header.ftm.bitsPerSample != 16))
		{
			throw std::runtime_error("Unsupported wav format");
		}
	}
};