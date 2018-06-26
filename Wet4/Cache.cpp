#include "Cache.hpp"

/*
 * createBitMask - creates a bit mask in a specific size
 * param[in] size - the wanted mask size (in bits)
 */
static unsigned long createBitMask(unsigned size) {
	unsigned long mask = 0;
	for (int i = 0; i < size; ++i) {
		mask |= 1 << i;
	}
	return mask;
}

unsigned long Cache::extractSet (unsigned long address) {
	unsigned long set = 0;
	unsigned long shifted_addr = address >> blockSize;
	if (setBits == 0)
		return set;
	unsigned long bit_mask = createBitMask(setBits);
	set = shifted_addr & bit_mask;
	return set;
}

unsigned long Cache::extractTag (unsigned long address) {
	unsigned long tag = 0;
	unsigned long shifted_addr = address >> blockSize;
	if (tagBits == 0)
		return tag;
	unsigned long bit_mask = createBitMask(tagBits);
	tag = shifted_addr & bit_mask;
	return tag;
}

void Cache::updateLRU (unsigned long int setNumber ,int wayNumber) {
	for (_List_iterator<int> it = LRUs[setNumber].begin() ;
			it < LRUs[setNumber].end() ; it++) {
		if (*it == wayNumber) {
			LRUs[setNumber].erase(it);
			LRUs[setNumber].push_back(wayNumber);
		}
	}
}

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
	for (int i = 0; i < nWays; i++) { // loop all the ways until you find the matching tagNumber
		if (Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			hits++;
			updateLRU(setNumber, tagNumber);
			return true;
		}
	}
	return false;
}

void Cache::setLineDirty(unsigned long address) {
	unsigned long setNumber = extractSet(address); // get the set number of the address
	unsigned long tagNumber = extractTag(address); // get the tag number of the address
	for (int i = 0; i < nWays; i++) { // loop all the ways until you find the matching tagNumber
		if (Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			Ways[i].find(setNumber)->second.setLineDirtyBit(true);
			updateLRU(setNumber, i);
			break;
		}
	}
}

bool Cache::setIsAvailable(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.getLineTag() == -1) {
			return true;
		}
	}
	return false;
}


unsigned long Cache::lineToRemove(unsigned long address,bool isRead) {
	unsigned long setNumber = extractSet(address);
	_List_iterator<int> it = LRUs[setNumber].begin();
	return (Ways[*it].find(setNumber)->second.getLine());
}

void Cache::removeAddress(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	unsigned long tagNumber = extractTag(address);
	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			Ways[i].find(setNumber)->second.setLineDirtyBit(false);
			Ways[i].find(setNumber)->second.setLineTag(-1);

			for (_List_iterator<int> it = LRUs[setNumber].begin() ;
					it < LRUs[setNumber].end() ; it++) {
				if (*it == i) {
					LRUs[setNumber].erase(it);
				}
			}

			break;
		}
	}
}

void Cache::insertAddress(unsigned long address, bool isRead) {
	unsigned long setNumber = extractSet(address);
	unsigned long tagNumber = extractTag(address);

}

bool Cache::isDirty(unsigned long address) {
	unsigned long setNumber = extractSet(address);
	unsigned long tagNumber = extractTag(address);
	for (int i = 0; i < nWays; i++) {
		if (Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			return Ways[i].find(setNumber)->second.getLineDirtyBit(); 	//return true if line is dirty. otherwise, false
		}
	}
	return false;
}
