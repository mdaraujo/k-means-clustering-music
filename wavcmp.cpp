#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <sndfile.hh>

using namespace std;

int validateFile(const SndfileHandle &sndFile)
{
	if (sndFile.error())
	{
		cerr << "Error: invalid file" << endl;
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
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		cerr << "Usage: wavcmp <input file> <original file>" << endl;
		return 1;
	}

	string inputFileName = argv[argc - 2];
	cout << "Reading input file " << inputFileName << " ..." << endl;
	SndfileHandle inputFile{inputFileName};

	if (validateFile(inputFile) == 1)
		return 1;

	vector<short> inputSamples(inputFile.frames() * inputFile.channels());
	int nFrames = inputFile.readf(inputSamples.data(), inputFile.frames() * inputFile.channels());

	if (nFrames != inputFile.frames())
	{
		cerr << "Error reading input file" << endl;
		return 1;
	}

	string originalFileName = argv[argc - 1];
	cout << "Reading original file " << originalFileName << " ..." << endl;
	SndfileHandle originalFile{originalFileName};

	if (validateFile(originalFile) == 1)
		return 1;

	vector<short> originalSamples(originalFile.frames() * originalFile.channels());
	nFrames = originalFile.readf(originalSamples.data(), originalFile.frames() * originalFile.channels());

	if (nFrames != originalFile.frames())
	{
		cerr << "Error reading original file" << endl;
		return 1;
	}

	if (inputFile.frames() != originalFile.frames())
	{
		cerr << "Error: the files do not have the same number of frames" << endl;
		return 1;
	}

	double signalEnergy = 0;
	double noiseEnergy = 0;
	double error;
	double maxError = 0;
	for (size_t i = 0; i < originalSamples.size(); i++)
	{
		signalEnergy += pow(originalSamples[i], 2);

		error = abs(originalSamples[i] - inputSamples[i]);
		noiseEnergy += pow(error, 2);

		if (error > maxError)
		{
			maxError = error;
		}
	}
	cout << "Signal Energy: " << signalEnergy << endl;
	cout << "Noise Energy: " << noiseEnergy << endl;

	double snr = 10 * log10(signalEnergy / noiseEnergy);

	cout << "SNR: " << snr << endl;

	cout << "Max Error: " << maxError << endl;

	return 0;
}
