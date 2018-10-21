#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <sndfile.hh>
#include <dirent.h>
#include "kmeans.h"

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		cerr << "Usage: wavhist <input file> <codebooks path>" << endl;
		return 1;
	}

	string inputFileName{argv[argc - 2]};
	SndfileHandle sndFile{inputFileName};
	if (sndFile.error())
	{
		cerr << "Error: invalid input file: " << inputFileName << endl;
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

	string codebooksPath{argv[argc - 1]};
	vector<vector<vector<short>>> codebooks;
	vector<tuple<string, string>> filesNames;
	vector<short> blocksSizes;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(codebooksPath.c_str())) != NULL)
	{
		cout << "Reading codebooks" << endl;
		size_t fileIdx = 0;
		while ((ent = readdir(dir)) != NULL)
		{
			string fname = {ent->d_name};

			if (fname.size() > 4 && fname.compare(fname.size() - 4, 4, ".txt") == 0)
			{
				string line;
				ifstream cbFile(codebooksPath + "/" + fname);
				if (cbFile.is_open())
				{
					int codebookSize = -1;
					int lineIdx = -1;
					while (getline(cbFile, line))
					{
						size_t pos = 0;
						string value;
						string delimiter = ",";
						int valueIdx = 0;

						if (lineIdx == -1)
						{
							while ((pos = line.find(delimiter)) != string::npos)
							{
								value = line.substr(0, pos);
								if (valueIdx == 0) // blocksize
									blocksSizes.push_back(stoi(value));
								else if (valueIdx == 2) // codebooksize
									codebookSize = stoi(value);
								else if (valueIdx == 5) // file name
									filesNames.push_back(make_tuple(fname, value));

								line.erase(0, pos + delimiter.length());
								valueIdx++;
							}
							if (codebookSize == -1)
							{
								cerr << "Invalid codebook file structure" << endl;
								return 1;
							}
							codebooks.push_back(vector<vector<short>>(codebookSize));
							lineIdx++;
							continue;
						}

						codebooks[fileIdx][lineIdx] = vector<short>(blocksSizes[fileIdx]);

						while ((pos = line.find(delimiter)) != string::npos)
						{
							value = line.substr(0, pos);
							codebooks[fileIdx][lineIdx][valueIdx] = stoi(value);
							line.erase(0, pos + delimiter.length());
							valueIdx++;
						}
						lineIdx++;
					}
					cbFile.close();
					fileIdx++;
				}
				else
					cerr << "Unable to open file" << fname << endl;
			}
		}
		closedir(dir);
	}
	else
	{
		/* could not open directory */
		perror("");
		return EXIT_FAILURE;
	}

	vector<short> samples(sndFile.frames() * sndFile.channels());
	sndFile.readf(samples.data(), sndFile.frames() * sndFile.channels()); // read all at once ?
	// comparar com valor retorno readf

	cout << "Finding best match for " << inputFileName << endl;
	double minError = numeric_limits<double>::max();
	int betterCodebook = 0;
	double error;

	for (size_t cbIdx = 0; cbIdx < codebooks.size(); cbIdx++)
	{
		int blockSize = blocksSizes[cbIdx];
		error = 0.0;

		std::vector<short> block;
		for (size_t s = 0; s < samples.size(); s += blockSize)
		{
			block.clear();
			for (int j = 0; j < blockSize; j++)
			{
				block.push_back(samples[s + j]);
			}

			auto betterCluster = KMeans::findBetterCluster(codebooks[cbIdx], block);
			error += get<1>(betterCluster);
		}

		cout << "Evaluating file " << get<1>(filesNames[cbIdx]) << " from " << get<0>(filesNames[cbIdx]) << " - Error: " << error << endl;

		if (error < minError)
		{
			minError = error;
			betterCodebook = cbIdx;
		}
	}

	cout << inputFileName << " matches better with file " << get<1>(filesNames[betterCodebook]) << " from " << get<0>(filesNames[betterCodebook]) << endl;

	return 0;
}
