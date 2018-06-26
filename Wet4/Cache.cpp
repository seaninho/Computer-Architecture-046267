#include "Cache.hpp"

/*
 * createBitMask - creates a bit mask in a specific size
 * param[in] size - the wanted mask size (in bits)
 */
static unsigned long int createBitMask(unsigned int size) {
	unsigned long int mask = 0;
	for (int i = 0 ; i < size ; ++i) {
		mask |= 1 << i;
	}
	return mask;
}

unsigned long int Cache::extractSet (unsigned long int address) {
	unsigned long int set = 0;
	if (this->setBits = 0)
		return set;
	unsigned long int bit_mask = createBitMask(this->setBits);
	set = address & bit_mask;
	return set;
}

unsigned long int Cache::extractTag (unsigned long int address) {
	// TODO
	return 0;
}

double Cache::getMissRate() {
	double numOfMiss = this->totalQueries - this->numOfHits;
	return (numOfMiss / ((double)this->totalQueries));
}

unsigned int  Cache::getCycles() {
		return cycles;
}

bool Cache::hit(unsigned long int address, bool isRead){

}

void Cache::setLineDirty(unsigned long int address) {

}

bool Cache::setIsAvailable(unsigned long int address) {

}

unsigned long int Cache::lineToRemove(unsigned long int address,bool isRead) {

}

void Cache::removeAddress(unsigned long int address) {
	unsigned long int setNumber = getSetNumber (address); // get the set number of the address
	unsigned long int tagNumber = getTagNumber (address); // get the tag number of the address
	for (int i = 0 ; i  < nWays ; i++) { // loop all thw ways until you find the matching tagNumber
		if (Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			Ways[i].find(setNumber)->second.setLineDirtyBit(false);
			Ways[i].find(setNumber)->second.setLineTag(-1);
			Ways[i].find(setNumber)->second.setLine(0);
			break;
		}
	}
}

void Cache::insertAddress(unsigned long int address, bool isRead) {

}

bool Cache::isDirty(unsigned long int address) {
	unsigned long int setNumber = getSetNumber (address); // get the set number of the address
	unsigned long int tagNumber = getTagNumber (address); // get the tag number of the address
	for (int i = 0 ; i  < nWays ; i++) { // loop all thw ways until you find the matching tagNumber
		if (Ways[i].find(setNumber)->second.getLineTag() == tagNumber) {
			return Ways[i].find(setNumber)->second.getLineDirtyBit(); //return if the line at this set at the WAY#i is dirty
		}
	}
	return false;
}
