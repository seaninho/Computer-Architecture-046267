#ifndef CACHE_HPP_
#define CACHE_HPP_

#include <iostream>
#include <vector>
#include <map>
#include <list>
#include <cmath>
#include <utility>
#include "Line.hpp"

using namespace std;

class Cache {
private:
	unsigned size; 								// Cache size in bytes
	unsigned nWays;								// Associativity
	unsigned cycles;							// Number of cycles required to access cache
	unsigned blockSize;							// Number of bits to determine the offset
	unsigned setBits;							// Number of bits to determine the correct set
	unsigned tagBits;							// Number of bits to determine the correct tag
	unsigned hits;								// Number of total hits
	unsigned totalQueries;						// Number of total cache requests
	vector< map<unsigned, Line> > Ways;			// Cache's main data structure
	vector< list< int > > LRUs;					// Cache's LRU for each set

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
			map<unsigned, Line> temp_map;
			for (unsigned setNum = 0 ; setNum < setSize ; setNum++) {
				temp_map.insert(pair<unsigned, Line>(setNum, Line()));		// initializing the map
			}
			Ways.push_back(temp_map);		// adding the map to the ways vector
		}

		for (int setNum = 0 ; setNum < setSize ; setNum++) {
			list< int > emptyList;
			LRUs.push_back(emptyList);		// creating the set's LRU
		}
	}

	/*
	 * getMissRate - Returns cache's miss rate
	 *
	 */
	double getMissRate();

	/*
	 * getMissRate - Returns cache's time cycles needed to access data
	 *
	 */
	unsigned getCycles();

	/*
	 * insertAddress - Writing line to cache, meaning tag number to the right set
	 * param[in] address - line's address
	 */
	void insertAddress(unsigned long address);

	/*
	 * hit - Determining whether the cache has the line or not
	 * param[in] address - line's address
	 * 		returns true if the cache has it, false otherwise
	 */
	bool hit(unsigned long address);

	/*
	 * setLineDirty - Changes line's dirty bit, meaning the data has been rewritten
	 * param[in] address - line's address
	 */
	void setLineDirty(unsigned long address);

	/*
	 * setIsAvailable - Determines whether a specific set available (based on address given)
	 * param[in] address - line's address (contains set number)
	 * 		returns true if the set is available (in any way possible), false otherwise - which means a line needs to be removed
	 */
	bool setIsAvailable(unsigned long address);

	/*
	 * lineToRemove - Determines which line to remove from set
	 * param[in] address - line's address (contains set number)
	 * 		returns the address that needs to be removed from cache
	 */
	unsigned long lineToRemove(unsigned long address);

	/*
	 * removeAddress - Removes line from cache
	 * param[in] address - line's address
	 */
	void removeAddress(unsigned long address);

	/*
	 * isDirty - Determines if a line is dirty
	 * param[in] address - line's address (contains set number)
	 * 		returns true if the line is dirty, false otherwise
	 */
	bool isDirty(unsigned long address);


};





#endif /* CACHE_HPP_ */
