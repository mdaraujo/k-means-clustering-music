#ifndef BLOCK_H
#define BLOCK_H

#include <vector>

class Block
{
  private:
	std::vector<short> values;
	int clusterId;

  public:
	Block(const std::vector<short> &values)
	{
		this->values = values;
		this->clusterId = -1;
	}

	void setClusterId(int clusterId)
	{
		this->clusterId = clusterId;
	}

	int getClusterId()
	{
		return this->clusterId;
	}

	std::vector<short> getValues()
	{
		return this->values;
	}

};

#endif
