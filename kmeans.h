#ifndef KMEANS_H
#define KMEANS_H

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <assert.h>
#include <vector>
#include <tuple>
#include <math.h>
#include <limits>
#include "block.h"

class KMeans
{
  private:
	// lista dos vetores de comprimento blockSize
	std::vector<Block> blocks;
	// ponto central de cada cluster
	std::vector<std::vector<short>> centroids;
	int blockSize;
	int overlap;
	// k - numero de clusters (codebook size)
	int k;
	int maxIterations;

  public:
	KMeans(const int blockSize, const int overlap, const int k, const int maxIterations)
	{
		this->blockSize = blockSize;
		this->overlap = overlap;
		this->k = k;
		this->maxIterations = maxIterations;
		this->centroids.resize(k);
	}

	void update(const std::vector<short> &samples)
	{
		std::vector<short> blockValues;
		for (size_t i = 0; i < samples.size(); i += blockSize + overlap)
		{
			// define the block
			blockValues.clear();
			for (int j = 0; j < blockSize; j++)
			{
				blockValues.push_back(samples[i + j]);
			}
			blocks.push_back(Block(blockValues));
		}
	}

	std::vector<std::vector<short>> run()
	{
		// initialize centroids
		std::vector<short> choosedBlocks;
		for (int i = 0; i < k; i++)
		{
			// verify block was not already choosen
			while (true)
			{
				int randomBlock = rand() % (blocks.size() - 1);

				// if dont contains the block
				if (find(choosedBlocks.begin(), choosedBlocks.end(), randomBlock) == choosedBlocks.end())
				{
					choosedBlocks.push_back(randomBlock);
					centroids[i] = blocks[randomBlock].getValues();
					blocks[randomBlock].setClusterId(i);
					break;
				}
			};
		}

		int iterations = 0;
		int movedBlocks;
		double error;
		std::vector<std::vector<short>> newCentroids;
		std::vector<int> clusterSize;

		while (true)
		{
			iterations++;
			movedBlocks = 0;
			error = 0;
			newCentroids.clear();
			newCentroids.resize(k);
			clusterSize.clear();
			clusterSize.resize(k);

			// assign blocks to the better cluster
			for (size_t i = 0; i < blocks.size(); i++)
			{
				std::vector<short> blockValues = blocks[i].getValues();
				auto betterCluster = findBetterCluster(blockValues);
				int clusterIdx = std::get<0>(betterCluster);
				double dist = std::get<1>(betterCluster);

				// if (dist == 0)
				// 	std::cout << "Dist -- " << dist << "  Cluster -- " << clusterIdx << std::endl;

				if (clusterIdx != blocks[i].getClusterId())
				{
					movedBlocks++;
					blocks[i].setClusterId(clusterIdx);
				}
				error += dist;

				// sum this block values to the correspondent new cluster center
				if (newCentroids[clusterIdx].empty())
					newCentroids[clusterIdx].resize(blockSize);

				for (int j = 0; j < blockSize; j++)
				{
					newCentroids[clusterIdx][j] += blockValues[j];
				}
				clusterSize[clusterIdx]++;
			}

			if (movedBlocks == 0)
				break;

			if (iterations > maxIterations)
				break;

			// move centroids
			for (int i = 0; i < k; i++)
			{
				if (clusterSize[i] == 0)
				{
					// std::cout << "Cluster Empty ----- " << i << std::endl;
					int randomBlock = rand() % (blocks.size() - 1);
					centroids[i] = blocks[randomBlock].getValues();
					blocks[randomBlock].setClusterId(i);
					continue;
				}

				for (int j = 0; j < blockSize; j++)
				{
					centroids[i][j] = newCentroids[i][j] / clusterSize[i];
				}
			}

			std::cout << "Iteration " << std::setfill('0') << std::setw(3) << iterations;
			std::cout << ": " << std::setfill(' ') << std::setw(5) << movedBlocks << " moves"
					  << " -> Error: " << error << std::endl;
		};
		std::cout << "Finished !" << std::endl;
		return centroids;
	}

	std::tuple<int, double> findBetterCluster(const std::vector<short> &block)
	{
		double minDist = std::numeric_limits<double>::max();
		int betterCluster = 0;
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

	std::vector<short> getModifiedSamples()
	{
		std::vector<short> samples(blockSize * blocks.size());
		std::cout << blockSize * blocks.size() << std::endl;

		size_t sample_idx = 0;
		for (size_t i = 0; i < blocks.size(); i++)
		{
			int cluster = blocks[i].getClusterId();
			for (int j = 0; j < blockSize; j++)
			{
				samples[sample_idx++] = centroids[cluster][j];
			}
		}
		std::cout << sample_idx << std::endl;
		return samples;
	}
};

#endif
