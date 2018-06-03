/* 046267 Computer Architecture - Spring 2017 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <stdio.h>
#include <stdlib.h>
#include "dflow_calc.h"

typedef struct cmd_node cmdNode;
typedef struct exit_node exNode;

struct cmd_node{
	InstInfo* info;			// command information (opcode, dstIdx, src1Idx, src2Idx)
	unsigned duration;		// measured in clock cycles
	unsigned depth;
	cmdNode* dp1;
	cmdNode* dp2;
};

struct exit_node {
	cmdNode** cmd_pArray;
	unsigned depth;
};

ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[], unsigned int numOfInsts) {
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


