#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>
#include "Cache.hpp"

#define MAX_SIZE 32

using std::FILE;
using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ifstream;
using std::stringstream;

int main(int argc, char **argv) {

	if (argc < 19) {
		cerr << "Not enough arguments" << endl;
		return 0;
	}


	// Get input arguments

	// File
	// Assuming it is the first argument
	char* fileString = argv[1];
	ifstream file(fileString); //input file stream
	string line;
	if (!file || !file.good()) {
		// File doesn't hit or some other error
		cerr << "File not found" << endl;
		return 0;
	}

	unsigned MemCyc = 0, BSize = 0, L1Size = 0, L2Size = 0, L1Assoc = 0,
			L2Assoc = 0, L1Cyc = 0, L2Cyc = 0, WrAlloc = 0;

	for (int i = 2; i < 19; i += 2) {
		string s(argv[i]);
		if (s == "--mem-cyc") {
			MemCyc = atoi(argv[i + 1]);
		} else if (s == "--bsize") {
			BSize = atoi(argv[i + 1]);
		} else if (s == "--l1-size") {
			L1Size = atoi(argv[i + 1]);
		} else if (s == "--l2-size") {
			L2Size = atoi(argv[i + 1]);
		} else if (s == "--l1-cyc") {
			L1Cyc = atoi(argv[i + 1]);
		} else if (s == "--l2-cyc") {
			L2Cyc = atoi(argv[i + 1]);
		} else if (s == "--l1-assoc") {
			L1Assoc = atoi(argv[i + 1]);
		} else if (s == "--l2-assoc") {
			L2Assoc = atoi(argv[i + 1]);
		} else if (s == "--wr-alloc") {
			WrAlloc = atoi(argv[i + 1]);
		} else {
			cerr << "Error in arguments" << endl;
			return 0;
		}
	}

	unsigned int L1setBits = L1Size-BSize-L1Assoc;
	unsigned int L1tagBits = MAX_SIZE - L1setBits - BSize;
	Cache L1(L1Size, pow(2, L1Assoc), L1Cyc, BSize, L1setBits, L1tagBits);

	unsigned int L2setBits = L2Size-BSize-L2Assoc;
	unsigned int L2tagBits = MAX_SIZE - L2setBits - BSize;
	Cache L2(L2Size, pow(2, L2Assoc), L2Cyc, BSize, L2setBits, L2tagBits);

	int totalTime = 0;
	int totalCommands = 0;


	while (getline(file, line)) {

		stringstream ss(line);
		string address;
		char operation = 0; // read (R) or write (W)
		if (!(ss >> operation >> address)) {
			// Operation appears in an Invalid format
			cout << "Command Format error" << endl;
			return 0;
		}

//		cout << "totalCommands #" << totalCommands << " - " << operation;
		// DEBUG - remove this line
//		cout << "operation: " << operation;

		string cutAddress = address.substr(2); // Removing the "0x" part of the address

		// DEBUG - remove this line
//		cout << ", address (hex)" << cutAddress;

		unsigned long int num = 0;
		num = strtoul(cutAddress.c_str(), NULL, 16);

		// DEBUG - remove this line
//		cout << " (dec) " << num; // << endl;

		totalCommands++;

		bool isRead = false;
		if (operation == 'r') {
			isRead = true;
		}

		if (L1.hit(num)) {
//			cout << " - L1 hit " << endl;
			if (!isRead) {
				L1.setLineDirty(num);
			}
			totalTime += L1.getCycles();
		} else if (L2.hit(num)) {
//			cout << " - L2 hit" << endl;
			if (isRead || (!isRead && WrAlloc)) {
				if (!(L1.setIsAvailable(num))) {
					unsigned long oldNumL1 = L1.lineToRemove(num); // which line should be removeAddressd
					if (L1.isDirty(oldNumL1)) {
						L2.setLineDirty(oldNumL1);
					}
				}
				L1.insertAddress(num); // removeAddress line if necessary
				if (!isRead) {
					L1.setLineDirty(num);
				}
			} else {
				L2.setLineDirty(num); // change LRU
			}
			totalTime = totalTime + L1.getCycles() + L2.getCycles();
		} else { // miss at L1 & L2
//			cout << " - miss" << endl;
			if (isRead || (!isRead && WrAlloc)){
				if (!L2.setIsAvailable(num)) {
					unsigned long int oldNumL2 = L2.lineToRemove(num); // which line should be removeAddressd
					L1.removeAddress(oldNumL2);	// Snooping. if line exists in L1 cache, removeAddress it. if not, do nothing
				}
				L2.insertAddress(num);
				if (!(L1.setIsAvailable(num))) {  // check if set is available
//					cout << "cachSim.cpp : line 147" << endl;
					unsigned long int oldNumL1 = L1.lineToRemove(num); // which line should be removeAddressd
//					cout << "cachSim.cpp : line 149" << endl;
					if (L1.isDirty(oldNumL1)) {
//						cout << "cachSim.cpp : line 151" << endl;
						L2.setLineDirty(oldNumL1);
					}
				}
//				cout << "cachSim.cpp : line 156" << endl;
				L1.insertAddress(num);
				if (!isRead) {
					L1.setLineDirty(num);
				}
			} else {

			}
			totalTime = totalTime + L1.getCycles() + L2.getCycles() + MemCyc;
		}
	}

	double L1MissRate = L1.getMissRate();
	double L2MissRate = L2.getMissRate();
	double avgAccTime = ((double)totalTime)/((double)totalCommands);

	printf("L1miss=%.03f ", L1MissRate);
	printf("L2miss=%.03f ", L2MissRate);
	printf("AccTimeAvg=%.03f\n", avgAccTime);

	return 0;
}
