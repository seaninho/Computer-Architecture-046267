#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <vector>
#include <map>
#include <list>
#include <cmath>
#include <utility>
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
	vector< map<unsigned, Line> > Ways;
	vector< list< int > > LRUs;

	unsigned long extractSet(unsigned long address);
	unsigned long extractTag(unsigned long address);
	void updateLRU(unsigned long setNumber ,int wayNumber);
public:
	Cache(unsigned size, unsigned nWays, unsigned cycles, unsigned blockSize,
			unsigned setBits, unsigned tagBits) :
				size(size), nWays(nWays), cycles(cycles), blockSize(blockSize),
				setBits(setBits), tagBits(tagBits), hits(0), totalQueries(0) {
		int setSize = pow(2, (double)setBits);
		for (int i = 0 ; i < nWays ; i++) {
			map<unsigned, Line> Ways[i];
			for (int setNum = 0 ; setNum < setSize ; setNum) {
				Line empty_line;
				Ways[i].insert(pair<unsigned, Line>(setNum, empty_line));
			}
		}
	}

	double getMissRate();

	unsigned getCycles();

	void insertAddress(unsigned long address);

	bool hit(unsigned long address);

	void setLineDirty(unsigned long address);

	bool setIsAvailable(unsigned long address);

	unsigned long lineToRemove(unsigned long address);

	void removeAddress(unsigned long address);

	bool isDirty(unsigned long address);


};





#endif /* CACHE_HPP_ */
