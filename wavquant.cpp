#include <iostream>
#include <vector>
#include <string>
#include <sys/stat.h>
#include <sndfile.hh>
#include "wavquant.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[])
{

	if (argc < 4)
	{
		cerr << "Usage: wavhist <input file> <number of bits> <channel?>" << endl;
		return 1;
	}

	string fileName{argv[argc - 3]};
	SndfileHandle sndFile{fileName};
	if (sndFile.error())
	{
		cerr << "Error: invalid input file" << endl;
		return 1;
	}

	if ((sndFile.format() & SF_FORMAT_TYPEMASK) != SF_FORMAT_WAV)
	{
		cerr << "Error: file is not in WAV format" << endl;
		return 1;
	}

	if ((sndFile.format() & SF_FORMAT_SUBMASK) != SF_FORMAT_PCM_16)
	{
		cerr << "Error: file is not in PCM_16 format" << endl;
		return 1;
	}

	int numBits{stoi(argv[argc - 2])};
	// TODO: Validar numero de bits - n <= 16

	// TODO: if channel not passed as arg, dont dump text
	int channel{stoi(argv[argc - 1])};
	if (channel >= sndFile.channels())
	{
		cerr << "Error: invalid channel requested" << endl;
		return 1;
	}

	fileName = fileName.substr(0, fileName.find("."));
	string outFolder = "quants";
	if (mkdir(outFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
	{
		// error is not already exists
		if (errno != EEXIST)
		{
			cout << "cannot create quants output folder" << endl;
			return 1;
		}
	}

	WAVQuant quant{sndFile, numBits, outFolder + "/" + fileName};

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
	while ((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE)))
	{
		samples.resize(nFrames * sndFile.channels());
		quant.update(samples);
	}

	// TODO: if channel not passed as arg, dont dump text
	quant.dump_to_text_file(channel);

	quant.dump_to_wav_file();

	return 0;
}
