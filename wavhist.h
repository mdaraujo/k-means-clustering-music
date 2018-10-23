#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <sndfile.hh>

class WAVHist
{
  private:
	// contagem de quantas vezes aparece cada valor
	// contagem por cada channel de audio - cada mapa
	std::vector<std::map<short, size_t>> counts;
	std::string fileName;
	int numChannels;

  public:
	WAVHist(const SndfileHandle &sfh, const std::string fileName)
	{
		numChannels = sfh.channels();
		counts.resize(numChannels);
		this->fileName = fileName;
	}

	void update(const std::vector<short> &samples)
	{
		size_t n{};
		for (auto s : samples)
			counts[n++ % counts.size()][s]++;
	}

	void dump(const size_t channel) const
	{
		for (auto [value, counter] : counts[channel])
			std::cout << value << '\t' << counter << '\n';
	}

	void dump_channel_to_file(const size_t channel) const
	{
		std::ofstream myfile;
		myfile.open(fileName + "_hist_ch_" + std::to_string(channel) + ".txt");
		for (auto [value, counter] : counts[channel])
			myfile << value << '\t' << counter << '\n';
		myfile.close();
	}

	void dump_mono_to_file() const
	{
		std::ofstream myfile;
		myfile.open(fileName + "_hist_mono.txt");

		std::map<short, size_t> monoCounts;

		for (auto channel : counts)
		{
			for (auto [value, counter] : channel)
			{
				monoCounts[value] += counter;
			}
		}

		for (auto [value, counter] : monoCounts)
			myfile << value << '\t' << (double)counter / numChannels << '\n';

		myfile.close();
	}
};

#endif
