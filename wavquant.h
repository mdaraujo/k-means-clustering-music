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
		std::string filename = fileName + "_quant_nbits" + std::to_string(numBits) + "_ch" + std::to_string(channel) + ".txt";
		file.open(filename);

		double sec = 0;
		double inc = (double)1 / samplerate;
		//std::cout.precision(8);
		for (auto value : channels[channel])
		{
			file << std::fixed << sec << '\t' << value << '\n';
			sec += inc;
		}
		file.close();
	}

	void dump_to_wav_file() const
	{
		SndfileHandle sndFileOut{fileName + "_quant_nbits" + std::to_string(numBits) + ".wav",
								 SFM_WRITE, format, numChannels, samplerate};

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
			// TODO: generalizar para qlq numero de canais
			samples[sample_idx++] = channels[0][i];
			samples[sample_idx++] = channels[1][i];
		}
		std::cout << i << std::endl;
		std::cout << sample_idx << std::endl;
		std::cout << numFrames << std::endl;

		sndFileOut.writef(samples.data(), numFrames);
	}
};

#endif
