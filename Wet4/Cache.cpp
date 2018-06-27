#include "Cache.hpp"

/*******		Static function			*******/

/*
 * createBitMask - Creates a bit mask in a specific size
 * param[in] size - the wanted mask size (in bits)
 */
static unsigned long createBitMask(unsigned size) {
	unsigned long mask = 0;
	for (int i = 0; i < size; ++i) {
		mask |= 1 << i;
	}
	return mask;
}

/*******		Private methods			*******/

/*
 * extractSet - Extracts set number from address
 * param[in] address - line's address
 */
unsigned long Cache::extractSet (unsigned long address) {
	unsigned long set = 0;
	unsigned long shifted_addr = address >> blockSize;				// shifting the correct amount of bits
	if (setBits == 0)
		return set;
	unsigned long bit_mask = createBitMask(setBits);				// creating the correct bit mask according to number of set bits
	set = shifted_addr & bit_mask;
	return set;
}

/*
 * extractTag - Extracts stag number from address
 * param[in] address - line's address
 */
unsigned long Cache::extractTag (unsigned long address) {
	unsigned long tag = 0;
	unsigned long shifted_addr = address >> (blockSize + setBits);		// shifting the correct amount of bits
	if (tagBits == 0)
		return tag;
	unsigned long bit_mask = createBitMask(tagBits);					// creating the correct bit mask according to number of tag bits
	tag = shifted_addr & bit_mask;
	return tag;
}

/*
 * updateLRU - Updates set's LRU
 * param[in] setNumber - number of set's LRU
 * param[in] wayNumber - number of way
 */
void Cache::updateLRU (unsigned long int setNumber ,int wayNumber) {
	for (_List_iterator<int> it = LRUs[setNumber].begin();
			it != LRUs[setNumber].end(); it++) {
		if (*it == wayNumber) {
			LRUs[setNumber].erase(it);
			break;
		}
	}
	LRUs[setNumber].push_back(wayNumber);
}

/*******		Public methods			*******/

double Cache::getMissRate() {
	double misses = totalQueries - hits;
	return (misses / ((double)totalQueries));
}

unsigned Cache::getCycles() {
	return cycles;
}

bool Cache::hit(unsigned long address){
	totalQueries++;
	unsigned long setNumber = extractSet(address); // get the set number of the address
	unsigned long tagNumber = extractTag(address); // get the tag number of the address

	// loop all the ways until you find the matching tagNumber
	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.isInit() &&
				Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			hits++;
			updateLRU(setNumber, i);
			return true;
		}
	}

	return false;
}

void Cache::setLineDirty(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	unsigned long tagNumber = extractTag(address);
	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.isInit() &&
				Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			Ways[i].find(setNumber)->second.setLineDirtyBit(true);		// writing data to a line the cache has - dirty bit needs to change
			updateLRU(setNumber, i);		// updating LRU since the line has been accessed
			break;
		}
	}
}

bool Cache::setIsAvailable(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.isInit() == false) {
			return true;		// if line is uninitialized, there is an available set
		}
	}
	return false;		// otherwise, set has all its lines occupied
}


unsigned long Cache::lineToRemove(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	_List_iterator<int> it = LRUs[setNumber].begin();

	for (int j = 0 ; j<pow(2,setBits) ; j++ ) {
		for (_List_iterator<int> i = LRUs[j].begin(); i != LRUs[j].end(); i++) {
		}
	}

	return (Ways[*it].find(setNumber)->second.getLineAddr());
}

void Cache::removeAddress(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	unsigned long tagNumber = extractTag(address);

	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.isInit() &&
				Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			Ways[i].find(setNumber)->second.setLineDirtyBit(false);
			Ways[i].find(setNumber)->second.setLineTag(0);
			Ways[i].find(setNumber)->second.setInit(false);

			for (_List_iterator<int> it = LRUs[setNumber].begin();
					it != LRUs[setNumber].end(); it++) {
				if (*it == i) {
					LRUs[setNumber].erase(it);
					break;
				}
			}

			break;
		}
	}
}

void Cache::insertAddress(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	unsigned long tagNumber = extractTag(address);
	int wayNumber = -1;
	// determining if there is a vacancy
	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.isInit() == false) {
			wayNumber = i;
			break;
		}
	}

	if (wayNumber < 0) {
		wayNumber = LRUs[setNumber].front();
		LRUs[setNumber].pop_front();
	}
	// adding the address to cache
	Ways[wayNumber].find(setNumber)->second.setLineAddr(address);
	Ways[wayNumber].find(setNumber)->second.setLineTag(tagNumber);
	Ways[wayNumber].find(setNumber)->second.setLineDirtyBit(false);
	Ways[wayNumber].find(setNumber)->second.setInit(true);
	// updating the LRU
	LRUs[setNumber].push_back(wayNumber);
}

bool Cache::isDirty(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	unsigned long tagNumber = extractTag(address);

	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.isInit() &&
				Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			return Ways[i].find(setNumber)->second.getLineDirtyBit(); 	//return true if line is dirty. otherwise, false
		}
	}

	return false;
}
