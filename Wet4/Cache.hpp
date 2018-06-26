#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <vector>
#include <map>
#include <list>
#include <cmath>
#include "Line.hpp"

using namespace std;

class Cache {
private:
	unsigned size; 			// Cache size in bytes
	unsigned nWays;			// Associativity
	unsigned cycles;		// Number of cycles required to access cache
	unsigned blockSize;
	unsigned setBits;		// Number of bits to determine the correct set
	unsigned tagBits;		// Number of bits to determine the correct tag
	unsigned hits;
	unsigned totalQueries;
	vector<map<unsigned, Line>> Ways;
	vector<list<unsigned>> LRUs;

	unsigned long extractSet(unsigned long address);
	unsigned long extractTag(unsigned long address);
	void updateLRU(unsigned long setNumber ,unsigned long tagNumber);
public:
	Cache(unsigned size, unsigned nWays, unsigned cycles, unsigned blockSize,
			unsigned setBits, unsigned tagBits,	unsigned hits, unsigned totalQueries) :
				size(size), nWays(nWays), cycles(cycles), blockSize(blockSize),
				setBits(setBits), tagBits(tagBits), hits(0), totalQueries(0) {}

	double getMissRate();

	unsigned getCycles();

	void insertAddress(unsigned long address, bool isRead);

	bool hit(unsigned long address);

	void setLineDirty(unsigned long address);

	bool setIsAvailable(unsigned long address);

	unsigned long lineToRemove(unsigned long address, bool isRead);

	void removeAddress(unsigned long address);

	bool isDirty(unsigned long address);


};





#endif /* CACHE_HPP_ */
