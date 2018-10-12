#ifndef KMEANS_H
#define KMEANS_H

#include <iostream>
#include <iomanip>
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
		for (int i = 0; i < k; i++)
		{
			std::vector<short> centroid; // random centroid
			for (int j = 0; j < blockSize; j++)
			{
				// generating random shorts - from -32768 to 32767
				centroid.push_back(rand() % 65535 + -32768);
			}
			centroids[i] = centroid;
		}
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
			//std::cout << i << std::endl;

			// put block in the better cluster
			auto betterCluster = findBetterCluster(block);
			clusters[std::get<0>(betterCluster)].push_back(block);
		}
	}

	void run()
	{
		unsigned int iterations = 0;
		unsigned int movedBlocks;
		double error;

		do
		{
			iterations++;
			movedBlocks = 0;
			error = 0;

			// move centroids
			for (size_t i = 0; i < clusters.size(); i++)
			{
				centroids[i] = findCentroid(clusters[i]);
				// printVector(centroids[i]);
			}

			// assign blocks to better cluster
			for (size_t i = 0; i < clusters.size(); i++)
			{
				for (size_t j = 0; j < clusters[i].size(); j++)
				{
					auto betterCluster = findBetterCluster(clusters[i][j]);
					unsigned int clusterIdx = std::get<0>(betterCluster);

					if (clusterIdx != i)
					{
						movedBlocks++;
						clusters[clusterIdx].push_back(clusters[i][j]);
						clusters[i].erase(clusters[i].begin() + j);
					}
					error += std::get<1>(betterCluster);
				}
			}

			std::cout << "Iteration " << std::setfill('0') << std::setw(3) << iterations;
			std::cout << ": " << movedBlocks << " moves"
					  << " -> Error: " << error << std::endl;

		} while (movedBlocks != 0);
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
		// TODO: assert vectors are the same size
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
			std::cout << i << ' ';
		std::cout << std::endl;
	}
};

#endif
