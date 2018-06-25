#include "Cache.hpp"

//****************** STATIC FUNCTION ******************//

unsigned long int getSetNumber (unsigned long int address) {
	// TODO
	return 0;
}

unsigned long int getTagNumber (unsigned long int address) {
	// TODO
	return 0;
}

//********************  FUNCTIONS ********************//
double Cache::getMissRate() {
	double numOfMiss = this->totalQueries - this->numOfHits;
	return (numOfMiss / ((double)this->totalQueries));
}

unsigned int  Cache::getCycles() {
		return cycles;
}

void Cache::insert(unsigned long int address) {
	unsigned long int setNumber = getSetNumber (address);
	unsigned long int tagNumber = getTagNumber (address);
	bool inserte = false;
	int size = static_cast<int>(this->Ways.size());
	for (int i = 0 ; i  < size ; i++) {
		if (Ways[i].find(setNumber)->second.getLineTag() == -1) {
			Ways[i].find(setNumber)->second.setLineTag(tagNumber);
			Ways[i].find(setNumber)->second.setLineDirtyBit(false);
			inserte = true;
			break;
		}
	}
	if (!inserte) {
		// TODO - noga
	}
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

}

void Cache::insertAddress(unsigned long int address, bool isRead) {

}

bool Cache::isDirty(unsigned long int address) {

}
