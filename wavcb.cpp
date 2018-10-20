#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include <sndfile.hh>
#include <stdio.h>
#include "kmeans.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[])
{
	srand(time(NULL));

	if (argc < 6)
	{
		cerr << "Usage: wavhist <input file> <block size> <overlap> <codebook size> <max iterations>" << endl;
		return 1;
	}

	string fileName{argv[argc - 5]};
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

	int blockSize{stoi(argv[argc - 4])};
	int overlap{stoi(argv[argc - 3])};
	int codebookSize{stoi(argv[argc - 2])};
	int maxIterations{stoi(argv[argc - 1])};
	// TODO: Validar inputs

	KMeans kmeans{blockSize, overlap, codebookSize, maxIterations};

	size_t nFrames;
	vector<short> samples(FRAMES_BUFFER_SIZE * sndFile.channels());
	while ((nFrames = sndFile.readf(samples.data(), FRAMES_BUFFER_SIZE)))
	{
		samples.resize(nFrames * sndFile.channels());
		kmeans.update(samples);
	}

	vector<vector<short>> codebook = kmeans.run();

	// dump codebook to file
	string outFolder = "codebooks";
	if (mkdir(outFolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
	{
		// error is not already exists
		if (errno != EEXIST)
		{
			cout << "cannot create codebooks output folder" << endl;
			return 1;
		}
	}

	string originalFileName = fileName;
	fileName = fileName.substr(0, fileName.find("."));
	string outFileName = fileName + "_" + to_string(blockSize) + "_" + to_string(overlap) + "_" + to_string(codebookSize) + "_" + to_string(maxIterations);
	ofstream outFile;
	outFile.open(outFolder + "/" + outFileName + ".txt");

	outFile << blockSize << ',';
	outFile << overlap << ',';
	outFile << codebookSize << ',';
	outFile << maxIterations << ',';
	outFile << kmeans.getError() << ',';
	outFile << originalFileName << "," << endl;

	for (auto block : codebook)
	{
		for (auto value : block)
			outFile << value << ',';
		outFile << '\n';
	}
	outFile.close();

	// test wav file
	vector<short> modifiedSamples = kmeans.getModifiedSamples();

	SndfileHandle sndFileOut{outFolder + "/" + outFileName + ".wav", SFM_WRITE,
							 sndFile.format(), sndFile.channels(), sndFile.samplerate()};

	if (sndFileOut.error())
	{
		std::cerr << "Error: invalid output file" << std::endl;
		return 1;
	}

	sndFileOut.writef(modifiedSamples.data(), sndFile.frames());

	return 0;
}
