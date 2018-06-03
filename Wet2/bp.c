/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "bp_api.h"

#define TARGET_SIZE 30				// maximum size of a target address (aligned)

typedef enum {
	SNT = 0,						// strong not taken
	WNT,							// weak not taken
	WT,								// weak taken
	ST								// strong taken
} StateMachine;

typedef struct {
	unsigned tag;
	unsigned target;
	bool* localBHR;					// local branch history register pointer
	StateMachine* localPredTable;	// local prediction table pointer
} BTBEntry;

typedef struct {
	unsigned btbSize;				// size measured in entries
	unsigned tagSize;				// size measured in bits
	unsigned historySize;			// size measured in bits
	unsigned predTableSize;			// size is 2^historySize
	bool historyIsGlobal;			// true - history is global; local otherwise
	bool* globalBHR;				// global branch history predictor pointer
	bool tableIsGlobal;				// true - tables is global; local otherwise
	StateMachine* globalPredTable;	// global prediction table pointer
	int Shared;						// not_using_share = 0, using_share_lsb = 1, using_share_mid = 2
	SIM_stats Stats;				// simulation stats
	BTBEntry* btbTable;				// BTB table pointer
} BP;

BP brPredictor; // Initializing the branch predictor

/*********************** static functions ***************************/

static bool isBTBTableSizeValid(unsigned btbSize) {
	if (btbSize != 1 && btbSize != 2 && btbSize != 4 && btbSize != 8 && btbSize != 16 && btbSize != 32)
		return false;
	else
		return true;
}

static bool isHistorySizeValid(unsigned historySize) {
	if (historySize < 1 || historySize > 8)
		return false;
	else
		return true;
}

static bool isTagSizeValid(unsigned tagSize) {
	if (tagSize < 0 || tagSize > 30)
		return false;
	else
		return true;
}

/*
 * cleanBTBTable - Initializes the BTB table
 */
static void cleanBTBTable() {
	// cleaning each BTBEntry
	for (int i = 0; i < brPredictor.btbSize; ++i) {

		brPredictor.btbTable[i].tag = 0;
		brPredictor.btbTable[i].target = 0;

		// initializing local branch history register according to isGlobalHist
		if (brPredictor.historyIsGlobal) {
			brPredictor.btbTable[i].localBHR = NULL;
		} else {
			// initializing each local branch history register pointer to point at NULL
			for (int j = 0; j < brPredictor.historySize; ++j) {
				brPredictor.btbTable[i].localBHR[j] = 0;
			}
		}

		// initializing local prediction table according to isGlobalTable
		if (brPredictor.tableIsGlobal) {
			brPredictor.btbTable[i].localPredTable = NULL;
		} else {
			// initializing each local prediction table pointer to point at NULL
			for (int j = 0; j < brPredictor.predTableSize; ++j) {
				brPredictor.btbTable[i].localPredTable[j] = WNT;
			}
		}
	}
}

/*
 * createBitMask - creates a bit mask in a specific size
 * param[in] size - the wanted mask size (in bits)
 */
static unsigned createBitMask(unsigned size) {
	unsigned mask = 0;
	for (unsigned i = 0 ; i < size ; ++i) {
		mask |= 1 << i;
	}
	return mask;
}

/*
 * calcTableIndex - calculates the prediction table index
 * param[in] BHR - a pointer to the branch history register used to calculate the index
 * param[in] pc - current program counter
 */
static unsigned calcTableIndex(bool* BHR, unsigned pc) {
	unsigned index = 0, curr = 1;
	for (int i = 0; i < brPredictor.historySize; ++i) {
		if (BHR[i]) {
			index += curr;
		}
		curr *= 2;
	}

	// if we use a local prediction table or Shared == 0
	if (!brPredictor.tableIsGlobal || brPredictor.Shared == 0) {
		return index;
	}

	// if we use a global prediction table & Shared != 0
	unsigned shifted_pc = 0;
	if (brPredictor.Shared == 1) {
		shifted_pc = pc >> 2;
	}
	else {
		shifted_pc = pc >> 16;
	}

	unsigned bit_mask = createBitMask(brPredictor.historySize);
	unsigned shared_bits = shifted_pc & bit_mask;
	return index ^ shared_bits;
}

/*
 * flushResolve - determines whether a pipe flush needs to happen or not
 * param[in] state - the branch history register used to calculate the index
 * param[in] taken - the actual decision, true if taken and false if not taken
 * param[in] diff_tag - true if tags are different, false otherwise
 * param[in] targetPc - the branch instruction target address
 * param[in] pred_dst - the predicted target address
 */
static bool flushResolve(StateMachine state, bool taken, bool diff_tag, unsigned targetPc, unsigned pred_dst) {
	if (diff_tag) {
		if (taken)
			return true;
		else
			return false;
	}
	else {
		if (((state == WNT || state == SNT) && taken) || ((state == WT || state == ST) && !taken) || (taken && targetPc != pred_dst))
			return true;
		else
			return false;
	}
}


/*
 * updateBHR - updates the branch history register
 * param[in] BHR - a pointer to the branch history register that needs to be updated
 * param[in] taken - the actual decision, true if taken and false if not taken
 */
static void updateBHR(bool* BHR, bool taken){
	for (int i = brPredictor.historySize; i > 0; --i) {
		BHR[i] = BHR[i-1];
	}
	BHR[0] = taken;
}

/****************** bp_api.h functions implementation ******************/

int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize,
		bool isGlobalHist, bool isGlobalTable, int Shared) {

	// checking BTB table size validity
	if (isBTBTableSizeValid(btbSize)) {
		brPredictor.btbSize = btbSize;
	} else {
		fprintf(stderr, "Error in input: Illegal BTBEntry table size\n");
		return -1;
	}

	// checking history size validity
	if (isHistorySizeValid(historySize)) {
		brPredictor.historySize = historySize;
	} else {
		fprintf(stderr, "Error in input: Illegal history size\n");
		return -1;
	}

	// checking tag size validity
	if (isTagSizeValid(tagSize)) {
		brPredictor.tagSize = tagSize;
	} else {
		fprintf(stderr, "Error in input: Illegal tag size\n");
		return -1;
	}

	brPredictor.historyIsGlobal = isGlobalHist;
	brPredictor.tableIsGlobal = isGlobalTable;
	brPredictor.Shared = Shared;
	// allocating space for the BTB according to the btbSize
	brPredictor.btbTable = malloc(sizeof(BTBEntry) * brPredictor.btbSize);
	// calculating the prediction table size
	brPredictor.predTableSize = pow(2, historySize);

	if (brPredictor.historyIsGlobal) {
		// allocating space for the global branch history register and initializing it
		brPredictor.globalBHR = malloc(sizeof(bool) * brPredictor.historySize);
		for (int i = 0; i < brPredictor.historySize; ++i) {
			brPredictor.globalBHR[i] = 0;
		}
		// initializing localBHR in cleanBTBTable
	} else {
		// initializing the global branch history register pointer to point at NULL
		brPredictor.globalBHR = NULL;
		// allocating space for a local branch history register in each btb entry
		for (int i = 0; i < brPredictor.btbSize; ++i) {
			brPredictor.btbTable[i].localBHR = malloc(sizeof(bool) * brPredictor.historySize);
			// initializing localBHR in cleanBTBTable
		}
	}

	if (brPredictor.tableIsGlobal) {
		// allocating space for the global prediction table and initializing it
		brPredictor.globalPredTable = malloc(sizeof(StateMachine) * brPredictor.predTableSize);
		for (int i = 0; i < brPredictor.predTableSize; ++i) {
			brPredictor.globalPredTable[i] = WNT;
		}
		// initializing local prediction table in cleanBTBTable
	} else {
		// initializing the global prediction table pointer to point at NULL
		brPredictor.globalPredTable = NULL;
		// allocating space for a local prediction table in each btb entry
		for (int i = 0; i < brPredictor.btbSize; ++i) {
			brPredictor.btbTable[i].localPredTable = malloc(sizeof(StateMachine) * brPredictor.predTableSize);
			// initializing local prediction table in cleanBTBTable
		}
	}

	cleanBTBTable();
	// reset the current simulator state
	brPredictor.Stats.flush_num = 0;
	brPredictor.Stats.br_num = 0;
	return 0;
}

bool BP_predict(uint32_t pc, uint32_t *dst) {
	unsigned shifted_pc = pc >> 2;
	unsigned bit_mask = createBitMask(log2(brPredictor.btbSize)); // creating the correct sized bit mask
	unsigned btb_entry = shifted_pc & bit_mask;		// calculating the correct BTB entry

	bit_mask = createBitMask(brPredictor.tagSize);  // creating the correct sized bit mask
	unsigned tag = shifted_pc & bit_mask;	// calculating the correct tag

	BTBEntry* entry = &brPredictor.btbTable[btb_entry];
	if (tag == entry->tag) {
		StateMachine state = WNT;
		// using global history & global table
		if (brPredictor.historyIsGlobal && brPredictor.tableIsGlobal) {
			state = brPredictor.globalPredTable[calcTableIndex(brPredictor.globalBHR, pc)];
		}
		// using local history & global table
		else if (!brPredictor.historyIsGlobal && brPredictor.tableIsGlobal) {
			state = brPredictor.globalPredTable[calcTableIndex(entry->localBHR, pc)];
		}
		// using global history & local table
		else if (brPredictor.historyIsGlobal && !brPredictor.tableIsGlobal) {
			state = entry->localPredTable[calcTableIndex(brPredictor.globalBHR, pc)];
		}
		// using local history & local table
		else if (!brPredictor.historyIsGlobal && !brPredictor.tableIsGlobal) {
			state = entry->localPredTable[calcTableIndex(entry->localBHR, pc)];
		}

		if (state == WT || state == ST) {
			*dst = brPredictor.btbTable[btb_entry].target;
			return true;
		}
	}
	*dst = pc + 4;
	return false;
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	unsigned shifted_pc = pc >> 2;
	unsigned bit_mask = createBitMask(log2(brPredictor.btbSize)); // creating the correct sized bit mask
	unsigned btb_entry = shifted_pc & bit_mask;		// calculating the correct BTB entry

	bit_mask = createBitMask(brPredictor.tagSize);  // creating the correct sized bit mask
	unsigned tag = shifted_pc & bit_mask;	// calculating the correct tag

	BTBEntry* entry = &brPredictor.btbTable[btb_entry];
	bool diff_tag = false;

	if (tag != entry->tag) {
		entry->tag = tag;
		diff_tag = true;
		// using local branch history register - needs to be initialized
		if (!brPredictor.historyIsGlobal) {
			for (int j = 0; j < brPredictor.historySize; ++j) {
				entry->localBHR[j] = 0;
			}
		}
		// using local prediction table - needs to be initialized
		if (!brPredictor.tableIsGlobal) {
			for (int j = 0; j < brPredictor.predTableSize; ++j) {
				entry->localPredTable[j] = WNT;
			}
		}
	}

	StateMachine state = WNT;
	// using global history & global table
	if (brPredictor.historyIsGlobal && brPredictor.tableIsGlobal) {
		// getting current state
		state = brPredictor.globalPredTable[calcTableIndex(brPredictor.globalBHR, pc)];
		if (flushResolve(state, taken, diff_tag, targetPc, pred_dst)) {
			brPredictor.Stats.flush_num++;
		}
		// if branch is taken and the current state is NOT strong taken we need to change it
		if (taken && state != ST) {
			brPredictor.globalPredTable[calcTableIndex(brPredictor.globalBHR, pc)]++;
		}
		// if branch is taken and the current state is NOT strong taken we need to change it
		else if (!taken && state != SNT) {
			brPredictor.globalPredTable[calcTableIndex(brPredictor.globalBHR, pc)]--;
		}
		updateBHR(brPredictor.globalBHR, taken);
	}
	// using local history & global table
	else if (!brPredictor.historyIsGlobal && brPredictor.tableIsGlobal) {
		// getting current state
		state = brPredictor.globalPredTable[calcTableIndex(entry->localBHR, pc)];
		if (flushResolve(state, taken, diff_tag, targetPc, pred_dst)) {
			brPredictor.Stats.flush_num++;
		}
		if (taken && state != ST) {
			brPredictor.globalPredTable[calcTableIndex(entry->localBHR, pc)]++;
		} else if (!taken && state != SNT) {
			brPredictor.globalPredTable[calcTableIndex(entry->localBHR, pc)]--;
		}
		updateBHR(entry->localBHR, taken);
	}
	// using global history & local table
	else if (brPredictor.historyIsGlobal && !brPredictor.tableIsGlobal) {
		// getting current state
		state = entry->localPredTable[calcTableIndex(brPredictor.globalBHR, pc)];
		if (flushResolve(state, taken, diff_tag, targetPc, pred_dst)) {
			brPredictor.Stats.flush_num++;
		}
		if (taken && state != ST) {
			entry->localPredTable[calcTableIndex(brPredictor.globalBHR, pc)]++;
		} else if (!taken && state != SNT) {
			entry->localPredTable[calcTableIndex(brPredictor.globalBHR, pc)]--;
		}
		updateBHR(brPredictor.globalBHR, taken);
	}
	// using local history & local table
	else if (!brPredictor.historyIsGlobal && !brPredictor.tableIsGlobal) {
		// getting current state
		state = entry->localPredTable[calcTableIndex(entry->localBHR, pc)];
		if (flushResolve(state, taken, diff_tag, targetPc, pred_dst)) {
			brPredictor.Stats.flush_num++;
		}
		if (taken && state != ST) {
			entry->localPredTable[calcTableIndex(entry->localBHR, pc)]++;
		} else if (!taken && state != SNT) {
			entry->localPredTable[calcTableIndex(entry->localBHR, pc)]--;
		}
		updateBHR(entry->localBHR, taken);
	}
	// changing target according to targetPc
	entry->target = targetPc;
	brPredictor.Stats.br_num++;
}

void BP_GetStats(SIM_stats *curStats) {

	curStats->flush_num = brPredictor.Stats.flush_num;
	curStats->br_num = brPredictor.Stats.br_num;

	// history and table is local
	if (!brPredictor.historyIsGlobal && !brPredictor.tableIsGlobal)	{

		brPredictor.Stats.size =
			brPredictor.btbSize * (brPredictor.tagSize + TARGET_SIZE + brPredictor.historySize + 2*brPredictor.predTableSize);
	}
	// history and table is global
	else if(brPredictor.historyIsGlobal && brPredictor.tableIsGlobal) {

		brPredictor.Stats.size =
			brPredictor.btbSize * (brPredictor.tagSize + TARGET_SIZE) + brPredictor.historySize + 2*brPredictor.predTableSize;
	}
	// history is local and table is global
	else if(!brPredictor.historyIsGlobal && brPredictor.tableIsGlobal) {

		brPredictor.Stats.size =
			brPredictor.btbSize * (brPredictor.tagSize + TARGET_SIZE + brPredictor.historySize) + 2*brPredictor.predTableSize;
	}
	// history is global and table is local
	else if(brPredictor.historyIsGlobal && !brPredictor.tableIsGlobal) {

		brPredictor.Stats.size =
			brPredictor.btbSize * (brPredictor.tagSize + TARGET_SIZE + 2*brPredictor.predTableSize) + brPredictor.historySize;
	}
	curStats->size = brPredictor.Stats.size;
	return;
}
