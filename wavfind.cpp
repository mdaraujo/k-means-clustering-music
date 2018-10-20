#include <iostream>
#include <fstream>
#include <vector>
#include <string.h>
#include <sndfile.hh>
#include <dirent.h>

using namespace std;

constexpr size_t FRAMES_BUFFER_SIZE = 65536; // Buffer for reading frames

int main(int argc, char *argv[])
{

	if (argc < 3)
	{
		cerr << "Usage: wavhist <input file> <codebooks path>" << endl;
		return 1;
	}

	string fileName{argv[argc - 2]};
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

	string codebooksPath{argv[argc - 1]};
	vector<vector<vector<short>>> codebooks;
	vector<string> filesName;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(codebooksPath.c_str())) != NULL)
	{
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
					int blockSize;
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
									blockSize = stoi(value);
								else if (valueIdx == 2) // codebooksize
									codebookSize = stoi(value);
								else if (valueIdx == 5) // file name
									filesName.push_back(value);

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

						codebooks[fileIdx][lineIdx] = vector<short>(blockSize);

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

	for (size_t i = 0; i < codebooks.size(); i++)
	{
		cout << filesName[i] << endl;
		int j = 0;
		for (auto blocks : codebooks[i])
		{
			cout << j << "  - ";
			for (auto value : blocks)
			{
				cout << value << ",";
			}
			cout << endl;
			j++;
		}
		cout << "\n\n"
			 << endl;
	}

	// fazer update para ler ficheiro por blocos
	// ver menor distancia de bloco para todos os vetores do codebook
	// associar bloco ao vetor code book mais proximo ??
	// somar as distancias minimas
	// escolher codebook que tem a distancia total minima

	return 0;
}
