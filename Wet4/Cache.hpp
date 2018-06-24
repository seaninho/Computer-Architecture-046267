#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <vector>
#include <map>
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
	bool isL1;					// True if cache is L1 cache, False otherwise
	vector<map<unsigned int, Line>> Ways;
};





#endif /* CACHE_HPP_ */
