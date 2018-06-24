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
	bool isL1;					// True if cache is L1 cache, False otherwise
	vector<map<unsigned int, Line>> Ways;
	queue<unsigned int> LRUq;

public:
	Cache(unsigned int size, unsigned int nWays, unsigned int cycles,
			unsigned int setBits, unsigned int tagBits, bool isL1,
				unsigned int numOfHits = 0, unsigned int totalQueries = 0) {}


};





#endif /* CACHE_HPP_ */
