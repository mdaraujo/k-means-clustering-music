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
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(codebooksPath.c_str())) != NULL)
	{
		size_t cbIdx = 0;
		while ((ent = readdir(dir)) != NULL)
		{
			string fname = {ent->d_name};

			if (fname.size() > 4 && fname.compare(fname.size() - 3, 3, ".txt"))
			{
				codebooks.push_back(vector<vector<short>>());
				string line;
				ifstream cbFile(codebooksPath + "/" + fname);
				if (cbFile.is_open())
				{
					size_t blockIdx = 0;
					while (getline(cbFile, line))
					{
						codebooks[cbIdx].push_back(vector<short>());
						size_t pos = 0;
						string value;
						string delimiter = ",";
						while ((pos = line.find(delimiter)) != string::npos)
						{
							value = line.substr(0, pos);
							codebooks[cbIdx][blockIdx].push_back(stoi(value));
							cout << value << endl;
							line.erase(0, pos + delimiter.length());
						}
						blockIdx++;
					}
					cbFile.close();
					cbIdx++;
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

	// fazer update para ler ficheiro por blocos
	// ver menor distancia de bloco para todos os vetores do codebook
	// associar bloco ao vetor code book mais proximo ??
	// somar as distancias minimas
	// escolher codebook que tem a distancia total minima

	return 0;
}
