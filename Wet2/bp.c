/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>


#define TARGET_SIZE 30
#define HISTORY_MAX_SIZE 30 //TODO 256?
#define BTB_MAX_SIZE 32


typedef enum {
	SNT = 0,
	WNT,
	WT,
	ST
} StateMachine;

typedef struct {
	uint32_t tag;
	uint32_t target;
	uint32_t* historyReg;
	StateMachine* prediction[HISTORY_MAX_SIZE];
} BTB;

typedef struct {
	unsigned btbSize;
	unsigned historySize;
	unsigned tagSize;
	bool isGlobalHist;
	bool isGlobalTable;
	int Shared;
	SIM_stats Stats;
	BTB btbTable[BTB_MAX_SIZE];
} BP;

BP BTBPredictor;
//StateMachine globalStateMachine[HISTORY_MAX_SIZE];

/***************************static functions***************************/
void cleanBTBTable() {
	for (int i = 0 ; i < BTBPredictor.btbSize ; i++) {
		BTBPredictor.btbTable[i].tag = -1;
		BTBPredictor.btbTable[i].target = 0;
		BTBPredictor.btbTable[i].historyReg = 0;
		for (int j = 0; j<HISTORY_MAX_SIZE; j++) {
			BTBPredictor.btbTable[i].prediction[j] = WT;
		}
	}
}

int calcMaxNumber(unsigned size) {
	int sum = 0;
	for (int i = 0 ; i < size ; i++) {
		sum += pow(2, i);
	}
	return sum;
}


/*************************** functions ***************************/
int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize,
             bool isGlobalHist, bool isGlobalTable, int Shared){
			if (btbSize != 1 && btbSize!=2 && btbSize !=4 && btbSize !=8 && btbSize !=16 && btbSize !=32 )	{
				fprintf(stderr, "Error in input: Illegal BTB table size\n");
				return -1;
			} else {
				BTBPredictor.btbSize = btbSize;
			}
			if (historySize > 8 || historySize < 1)	{
				fprintf(stderr, "Error in input: Illegal history size\n");
				return -1;
			}else{
				BTBPredictor.historySize = historySize;
			}
			if (tagSize > 30 || tagSize < 0) {
				fprintf(stderr, "Error in input: Illegaltag size\n");
				return -1;
			}else{
				BTBPredictor.tagSize = tagSize;
			}

			BTBPredictor.isGlobalHist = isGlobalHist;
			BTBPredictor.isGlobalTable = isGlobalTable;
			BTBPredictor.Shared = Shared;

			if (isGlobalHist) {
				uint32_t* history = malloc();
//				BTBPredictor
			} else {

			}
			if (isGlobalTable) {

			} else {

			}

			cleanBTBTable();

			// reset the currect simulator state
			BTBPredictor.Stats.flush_num = 0;
			BTBPredictor.Stats.br_num = 0;
			return 0;

}

bool BP_predict(uint32_t pc, uint32_t *dst){
	int tagMaxNum = calcMaxNumber(BTBPredictor.tagSize);
	unsigned tag_of_pc = (floor(pc/4))%tagMaxNum; // TODO update
	int i = 0;// TODO calculate the relevant btb line
	int predicted_target = -1;
	if (tag_of_pc == BTBPredictor.btbTable[i].tag)	{
		predicted_target = BTBPredictor.btbTable[i].target;
	}
	if (predicted_target == -1) {
		&dst = pc+4;
		return false;
	}
	if (BTBPredictor.isGlobalTable)	{
		if (BTBPredictor.isGlobalHist) {

		} else {

		}
	} else {

	}

	return false;
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){

	BTBPredictor.Stats.br_num++; // Increase the brach's counter



	return;
}

void BP_GetStats(SIM_stats *curStats) {
	curStats->flush_num = BTBPredictor.Stats.flush_num;
	curStats->br_num = BTBPredictor.Stats.br_num;

	int powerOfTwo = pow(2, BTBPredictor.historySize);

	if (!BTBPredictor.isGlobalHist && !BTBPredictor.isGlobalTable)	{
		BTBPredictor.Stats.size = BTBPredictor.btbSize*(BTBPredictor.tagSize+
				TARGET_SIZE+BTBPredictor.historySize+2*powerOfTwo);
	}else if(BTBPredictor.isGlobalHist && BTBPredictor.isGlobalTable) {
		BTBPredictor.Stats.size = BTBPredictor.btbSize*(BTBPredictor.tagSize+
				TARGET_SIZE)+BTBPredictor.historySize+2*powerOfTwo;
	}else if(!BTBPredictor.isGlobalHist && BTBPredictor.isGlobalTable) {
		BTBPredictor.Stats.size = BTBPredictor.btbSize*(BTBPredictor.tagSize+
				TARGET_SIZE+BTBPredictor.historySize)+2*powerOfTwo;
	}else if(BTBPredictor.isGlobalHist && !BTBPredictor.isGlobalTable) {
		BTBPredictor.Stats.size = BTBPredictor.btbSize*(BTBPredictor.tagSize+
				TARGET_SIZE+2*powerOfTwo)+BTBPredictor.historySize;
	}
	curStats->size = BTBPredictor.Stats.size;
	return;
}
