#include <iostream>
#include <vector>
#include <string>
#include <sndfile.hh>
#include "kmeans.h"

using namespace std;

//constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames
constexpr size_t FRAMES_BUFFER_SIZE = 136; // Buffer for reading frames

int main(int argc, char *argv[])
{

	if (argc < 5)
	{
		cerr << "Usage: wavhist <input file> <block size> <overlap> <codebook size>" << endl;
		return 1;
	}

	string fileName{argv[argc - 4]};
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

	int blockSize{stoi(argv[argc - 3])};
	int overlap{stoi(argv[argc - 2])};
	int codebookSize{stoi(argv[argc - 1])};
	// TODO: Validar inputs

	fileName = fileName.substr(0, fileName.find("."));
	KMeans kmeans{blockSize, overlap, codebookSize};

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
	while ((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE)))
	{
		samples.resize(nFrames * sndFile.channels());
		kmeans.update(samples);
		break; // for debug with less frames
	}

	kmeans.run();

	return 0;
}
