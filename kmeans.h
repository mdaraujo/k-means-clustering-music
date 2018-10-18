#ifndef KMEANS_H
#define KMEANS_H

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <assert.h>
#include <fstream>
#include <vector>
#include <tuple>
#include <math.h>
#include <limits>

class KMeans
{
  private:
	// lista de clusters
	// cada cluster tem o sua lista de pontos
	// os pontos tambem sao representados com um vetor
	std::vector<std::vector<std::vector<short>>> clusters;
	// ponto central de cada cluster
	std::vector<std::vector<short>> centroids;
	int blockSize;
	int overlap;
	// k - numero de clusters
	int k;

  public:
	KMeans(const int bs, const int ol, const int codebookSize)
	{
		blockSize = bs;
		overlap = ol;
		k = codebookSize;

		clusters.resize(k);
		centroids.resize(k);

		// initialize random centroids
		// for (int i = 0; i < k; i++)
		// {
		// 	std::vector<short> centroid; // random centroid
		// 	for (int j = 0; j < blockSize; j++)
		// 	{
		// 		// generating random shorts - from -32768 to 32767
		// 		centroid.push_back(rand() % 65535 + -32768);
		// 	}
		// 	centroids[i] = centroid;
		// }
	}

	void update(const std::vector<short> &samples)
	{
		// blockSize + overlap must be multiple of samples.size ??
		for (size_t i = 0; i < samples.size(); i += blockSize + overlap)
		{
			// define the block
			std::vector<short> block;
			for (int j = 0; j < blockSize; j++)
			{
				block.push_back(samples[i + j]);
			}
			clusters[0].push_back(block);
		}
	}

	std::vector<std::vector<short>> run()
	{
		unsigned int iterations = 0;
		unsigned int movedBlocks;
		double error;
		std::vector<std::tuple<unsigned int, unsigned int>> visitedBlocks;

		// initialize centroids
		for (int i = 0; i < k; i++)
		{
			int randomBlock = rand() % (clusters[0].size() - 1);
			centroids[i] = clusters[0][randomBlock];
		}

		while (true)
		{
			iterations++;
			movedBlocks = 0;
			error = 0;
			visitedBlocks.clear();

			// assign blocks to better cluster
			for (size_t i = 0; i < clusters.size(); i++)
			{
				for (size_t j = 0; j < clusters[i].size(); j++)
				{
					// check if block was already visited
					if (std::find(visitedBlocks.begin(), visitedBlocks.end(), std::make_tuple(i, j)) != visitedBlocks.end())
						continue;

					auto betterCluster = findBetterCluster(clusters[i][j]);
					unsigned int clusterIdx = std::get<0>(betterCluster);

					if (clusterIdx != i)
					{
						movedBlocks++;
						clusters[clusterIdx].push_back(clusters[i][j]);
						clusters[i].erase(clusters[i].begin() + j);

						visitedBlocks.push_back(std::make_tuple(clusterIdx, clusters[clusterIdx].size() - 1));

						// in case the erased block in cluster i, influence the position of a visited block
						for (size_t v = 0; v < visitedBlocks.size(); v++)
						{
							if (std::get<0>(visitedBlocks[v]) == i && std::get<1>(visitedBlocks[v]) > j)
								std::get<1>(visitedBlocks[v])--;
						}
					}
					error += std::get<1>(betterCluster);
				}
			}

			if (movedBlocks == 0)
				break;

			if (iterations > 100)
				break;

			// move centroids
			for (size_t i = 0; i < clusters.size(); i++)
			{
				centroids[i] = findCentroid(clusters[i]);
				//printVector(centroids[i]);
			}

			std::cout << "Iteration " << std::setfill('0') << std::setw(3) << iterations;
			std::cout << ": " << std::setfill(' ') << std::setw(5) << movedBlocks << " moves"
					  << " -> Error: " << error << std::endl;
		};
		return centroids;
	}

	std::vector<short> findCentroid(const std::vector<std::vector<short>> &cluster)
	{
		std::vector<short> centroid;
		centroid.resize(blockSize);

		if (cluster.size() == 0)
			return centroid;

		for (size_t i = 0; i < cluster.size(); i++)
		{
			for (size_t j = 0; j < cluster[i].size(); j++)
			{
				centroid[j] += cluster[i][j];
			}
		}
		for (size_t i = 0; i < centroid.size(); i++)
		{
			centroid[i] /= cluster.size();
		}
		return centroid;
	}

	std::tuple<unsigned int, double> findBetterCluster(const std::vector<short> &block)
	{
		double minDist = std::numeric_limits<double>::max();
		unsigned int betterCluster = 0;
		double dist = 0.0;
		for (size_t i = 0; i < centroids.size(); i++)
		{
			dist = euclideanDistance(centroids[i], block);
			if (dist < minDist)
			{
				minDist = dist;
				betterCluster = i;
			}
		}
		return std::make_tuple(betterCluster, minDist);
	}

	double euclideanDistance(const std::vector<short> &v1, const std::vector<short> &v2)
	{
		assert(v1.size() == v2.size());

		double result = 0.0;
		for (size_t i = 0; i < v1.size(); i++)
		{
			double dist = v1[i] - v2[i];
			result += dist * dist;
		}
		return result > 0.0 ? sqrt(result) : 0.0;
	}

	void printVector(std::vector<short> &v)
	{
		for (auto i : v)
			std::cout << std::setfill(' ') << std::setw(7) << i;
		std::cout << std::endl;
	}
};

#endif
