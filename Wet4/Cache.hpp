#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <vector>
#include <map>
#include <queue>
#include <cmath>
#include "Line.hpp"

using namespace std;

class Cache {
private:
	unsigned int size; 			// Cache size in bytes
	unsigned int nWays;			// Associativity
	unsigned int cycles;		// Number of cycles required to access cache
	unsigned int setBits;		// Number of bits to determine the correct set
	unsigned int tagBits;		// Number of bits to determine the correct tag
	unsigned int numOfHits;
	unsigned int totalQueries;
	vector<map<unsigned int, Line>> Ways;
	queue<unsigned int> LRUq;

public:
	Cache(unsigned int size, unsigned int nWays, unsigned int cycles,
			unsigned int setBits, unsigned int tagBits,
				unsigned int numOfHits = 0, unsigned int totalQueries = 0):
					size(size), nWays(nWays), cycles(cycles), setBits(setBits),
					tagBits(tagBits) {}

	double getMissRate();

	unsigned int  getCycles();

	void insertAddress(unsigned long int address, bool isRead);

	bool hit(unsigned long int address, bool isRead);

	void setLineDirty(unsigned long int address);

	bool setIsAvailable(unsigned long int address);

	unsigned long int lineToRemove(unsigned long int address,bool isRead);

	void removeAddress(unsigned long int address);

	bool isDirty(unsigned long int address);


};





#endif /* CACHE_HPP_ */
