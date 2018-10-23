#ifndef WAVQUANT_H
#define WAVQUANT_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <sndfile.hh>

class WAVQuant
{
  private:
	// lista de valores do sinal
	// listagem por cada channel de audio - cada lista
	std::vector<std::vector<short>> channels;
	int format;
	int numChannels;
	int samplerate;
	int numFrames;
	int numBits;
	std::string fileName;

  public:
	WAVQuant(const SndfileHandle &sfh, const int nBits, const std::string flName)
	{
		format = sfh.format();
		numChannels = sfh.channels();
		samplerate = sfh.samplerate();
		numFrames = sfh.frames();
		channels.resize(numChannels);
		numBits = nBits;
		fileName = flName;
	}

	void update(const std::vector<short> &samples)
	{
		int q = pow(2, 16) / (pow(2, numBits) - 1);
		size_t n{};
		for (auto s : samples)
		{
			channels[n++ % channels.size()].push_back(round(s / q) * q);
		}
	}

	void dump_to_text_file(const size_t channel) const
	{
		std::ofstream file;
		std::string outfilename = fileName + "_quant_nbits" + std::to_string(numBits) + "_ch" + std::to_string(channel) + ".txt";
		file.open(outfilename);

		double sec = 0;
		double inc = (double)1 / samplerate;
		for (auto value : channels[channel])
		{
			file << std::fixed << sec << '\t' << value << '\n';
			sec += inc;
		}
		file.close();
		std::cout << "Dump in " << outfilename << std::endl;
	}

	void dump_to_wav_file() const
	{
		std::string outfilename = fileName + "_quant_nbits" + std::to_string(numBits) + ".wav";
		SndfileHandle sndFileOut{outfilename, SFM_WRITE, format, numChannels, samplerate};

		if (sndFileOut.error())
		{
			std::cerr << "Error: invalid output file" << std::endl;
			return;
		}

		std::vector<short> samples(numFrames * numChannels);

		size_t i;
		size_t sample_idx = 0;
		for (i = 0; i < channels[0].size(); i++)
		{
			for (size_t j = 0; j < channels.size(); j++)
			{
				samples[sample_idx++] = channels[j][i];
			}
		}
		sndFileOut.writef(samples.data(), numFrames);
		std::cout << "Dump in " << outfilename << std::endl;
	}
};

#endif
