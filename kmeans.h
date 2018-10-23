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
	std::vector<std::vector<double>> centroids;
	int blockSize;
	int overlap;
	// k - numero de clusters (codebook size)
	int k;
	int maxIterations;
	double error;
	const int minimumErrorDiff = 2000;

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

	std::vector<std::vector<double>> run()
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
					centroids[i] = blocks[randomBlock].getDoubleValues();
					blocks[randomBlock].setClusterId(i);
					break;
				}
			};
		}

		int iterations = 0;
		double lastError = std::numeric_limits<double>::max();
		bool finished;
		std::vector<std::vector<double>> newCentroids;
		std::vector<int> clusterSize;

		while (true)
		{
			iterations++;
			finished = true;
			error = 0;
			newCentroids.clear();
			newCentroids.resize(k);
			clusterSize.clear();
			clusterSize.resize(k);

			// assign blocks to the better cluster
			for (size_t i = 0; i < blocks.size(); i++)
			{
				std::vector<short> blockValues = blocks[i].getValues();
				auto betterCluster = findBetterCluster(centroids, blockValues);
				int clusterIdx = std::get<0>(betterCluster);
				double dist = std::get<1>(betterCluster);

				if (clusterIdx != blocks[i].getClusterId())
				{
					blocks[i].setClusterId(clusterIdx);
					finished = false;
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

			std::cout << "Iteration " << std::setfill('0') << std::setw(3) << iterations;
			std::cout << " -> Error: " << std::setfill(' ') << std::setw(9) << error << std::endl;

			// have to stop before moving centroids,
			// so the blocks are with the better centroid and the wav file can be reproduced
			if (finished)
			{
				std::cout << "Zero blocks moved, convergion complete." << std::endl;
				break;
			}
			if (iterations >= maxIterations)
			{
				std::cout << "Maximum number of iterations reached." << std::endl;
				break;
			}
			if (lastError - error < minimumErrorDiff)
			{
				std::cout << "Minimum error difference of " << minimumErrorDiff << " reached." << std::endl;
				break;
			}

			lastError = error;

			// move centroids
			for (int i = 0; i < k; i++)
			{
				if (clusterSize[i] == 0)
				{
					// runs faster if a random block is choosen to occupy an empty cluser
					int randomBlock = rand() % (blocks.size() - 1);
					centroids[i] = blocks[randomBlock].getDoubleValues();
					blocks[randomBlock].setClusterId(i);
					continue;
				}

				for (int j = 0; j < blockSize; j++)
				{
					centroids[i][j] = newCentroids[i][j] / clusterSize[i];
				}
			}
		};

		return centroids;
	}

	static std::tuple<int, double> findBetterCluster(const std::vector<std::vector<double>> &centroids, const std::vector<short> &block)
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

	static double euclideanDistance(const std::vector<double> &v1, const std::vector<short> &v2)
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

	static void printCodebook(const std::vector<std::vector<short>> &codebook)
	{
		for (size_t i = 0; i < codebook.size(); i++)
		{
			std::cout << i << "  - ";
			printVector(codebook[i]);
		}
	}

	static void printVector(const std::vector<short> &v)
	{
		for (auto i : v)
			std::cout << std::setfill(' ') << std::setw(7) << i;
		std::cout << std::endl;
	}

	std::vector<short> getModifiedSamples()
	{
		std::vector<short> samples(blockSize * blocks.size());

		size_t sample_idx = 0;
		for (size_t i = 0; i < blocks.size(); i++)
		{
			int cluster = blocks[i].getClusterId();
			for (int j = 0; j < blockSize; j++)
			{
				samples[sample_idx++] = centroids[cluster][j];
			}
		}
		return samples;
	}

	double getError() const
	{
		return error;
	}
};

#endif
