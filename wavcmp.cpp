#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>
#include <sndfile.hh>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

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
		cerr << "Usage: wavhist <input file> <original file>" << endl;
		return 1;
	}

	string inputFileName = argv[argc - 2];
	cout << "Reading input file " << inputFileName << " ..." << endl;
	SndfileHandle inputFile{inputFileName};

	if (validateFile(inputFile) == 1)
		return 1;

	// dividir por channel ??
	vector<short> inputSamples(inputFile.frames() * inputFile.channels());
	inputFile.readf(inputSamples.data(), inputFile.frames() * inputFile.channels()); // read all at once ?
	// comparar com valor retorno readf

	string originalFileName = argv[argc - 1];
	cout << "Reading original file " << originalFileName << " ..." << endl;
	SndfileHandle originalFile{originalFileName};

	if (validateFile(originalFile) == 1)
		return 1;

	vector<short> originalSamples(originalFile.frames() * originalFile.channels());
	originalFile.readf(originalSamples.data(), originalFile.frames() * originalFile.channels());

	double signalEnergy = 0;
	double noiseEnergy = 0;
	for (size_t i = 0; i < originalSamples.size(); i++)
	{
		signalEnergy += pow(originalSamples[i], 2);
		noiseEnergy += pow(originalSamples[i] - inputSamples[i], 2);
	}
	cout << "Signal Energy: " << signalEnergy << endl;
	cout << "Noise Energy: " << noiseEnergy << endl;

	double snr = 10 * log10(signalEnergy / noiseEnergy);

	cout << "SNR: " << snr << endl;

	return 0;
}
