#ifndef WAVHIST_H
#define WAVHIST_H

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <sndfile.hh>

class WAVHist
{
  private:
	// contagem de quantas vezes aparece cada valor
	// contagem por cada channel de audio - cada mapa
	std::vector<std::map<short, size_t>> counts;

  public:
	WAVHist(const SndfileHandle &sfh)
	{
		counts.resize(sfh.channels());
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

	void dump_to_file(const size_t channel) const
	{
		std::ofstream myfile;
		myfile.open("hist.txt");
		for (auto [value, counter] : counts[channel])
			myfile << value << '\t' << counter << '\n';
		myfile.close();
	}
};

#endif
