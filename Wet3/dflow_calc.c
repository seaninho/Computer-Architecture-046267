/* 046267 Computer Architecture - Spring 2017 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dflow_calc.h"

#define ENTRYID -1

typedef struct cmd_node cmdNode;
typedef struct dp_graph DPGraph;

// command node (graph element) struct
struct cmd_node{
	int id;
	InstInfo* info;				// command information (opcode, dstIdx, src1Idx, src2Idx)
	unsigned int duration;		// measured in clock cycles
	unsigned int depth;
	cmdNode* dp1;				// a pointer to src1 dependency
	cmdNode* dp2;				// a pointer to src2 dependency
};

// dependency graph struct
struct dp_graph {
	cmdNode entry;				// graph's entry node
	cmdNode* cmd_pArray;		// graph's data structure
	unsigned int numOfInsts;
	unsigned int depth;
};

ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[], unsigned int numOfInsts) {
	// validating input
	if (!opsLatency || !progTrace || numOfInsts==0) {
		return PROG_CTX_NULL;
	}

	// allocating memory for the dependency graph
	DPGraph* graph = (DPGraph*)malloc(sizeof(DPGraph));
	if (!graph) {
		return PROG_CTX_NULL;	// in case malloc fails
	}
	// allocating memory for commands pointer array
	graph->cmd_pArray = (cmdNode*)malloc(numOfInsts*sizeof(cmdNode));
	if (!(graph->cmd_pArray)) {
		free(graph);
		return PROG_CTX_NULL;
	}
	graph->numOfInsts = numOfInsts;
	graph->depth = 0;

	// creating entry node
	graph->entry.depth = 0;
	graph->entry.duration = 0;
	graph->entry.dp1 = NULL;
	graph->entry.dp2 = NULL;
	graph->entry.depth = 0;
	graph->entry.id = ENTRYID;
	graph->entry.info = NULL;

	for (int i = 0 ; i < numOfInsts ; i++) {
		graph->cmd_pArray[i].id = i;
		// allocating sufficient memory for command's info
		graph->cmd_pArray[i].info = (InstInfo*)malloc(sizeof(InstInfo));
		if (!(graph->cmd_pArray[i].info)) {
			for (int j=0 ; j<i-1 ; j++ ) {
				free(graph->cmd_pArray[j].info);
			}
			free(graph->cmd_pArray);
			free(graph);
			return PROG_CTX_NULL;
		}
		// copying command info
		graph->cmd_pArray[i].info->dstIdx = progTrace[i].dstIdx;
		graph->cmd_pArray[i].info->opcode = progTrace[i].opcode;
		graph->cmd_pArray[i].info->src1Idx = progTrace[i].src1Idx;
		graph->cmd_pArray[i].info->src2Idx = progTrace[i].src2Idx;
		// extracting command's duration
		graph->cmd_pArray[i].duration = opsLatency[progTrace[i].opcode];

		graph->cmd_pArray[i].dp2 = NULL;
		graph->cmd_pArray[i].dp1 = NULL;
		graph->cmd_pArray[i].depth = 0;
		// only past commands determines dependencies
		for (int j = i-1 ; j >= 0; j--) {
			// checking src1 dependency
			if(graph->cmd_pArray[i].info->src1Idx == graph->cmd_pArray[j].info->dstIdx) {
				if (graph->cmd_pArray[i].dp1 == NULL) {
					graph->cmd_pArray[i].dp1 = &(graph->cmd_pArray[j]);
					// changing command's duration if need be
					if ((graph->cmd_pArray[i].depth) < (graph->cmd_pArray[j].depth+graph->cmd_pArray[j].duration)) {
						graph->cmd_pArray[i].depth = graph->cmd_pArray[j].depth+graph->cmd_pArray[j].duration;
					}
				}
			}
			// checking src2 dependency
			if (graph->cmd_pArray[i].info->src2Idx == graph->cmd_pArray[j].info->dstIdx) {
				if (graph->cmd_pArray[i].dp2 == NULL) {
					graph->cmd_pArray[i].dp2 = &(graph->cmd_pArray[j]);
					if ((graph->cmd_pArray[i].depth) < (graph->cmd_pArray[j].depth+graph->cmd_pArray[j].duration)) {
						graph->cmd_pArray[i].depth = graph->cmd_pArray[j].depth+graph->cmd_pArray[j].duration;
					}
				}

			}
		}
		// in case command has no dependencies at all
		if (graph->cmd_pArray[i].dp1 == NULL && graph->cmd_pArray[i].dp2 == NULL) {
			graph->cmd_pArray[i].dp1 = &(graph->entry);
		}
		// changing graph's depth in case we discovered a longer path
		if (graph->depth < (graph->cmd_pArray[i].depth+graph->cmd_pArray[i].duration)) {
			graph->depth = graph->cmd_pArray[i].depth+graph->cmd_pArray[i].duration;
		}
	}

    return graph;
}

void freeProgCtx(ProgCtx ctx) {
	if (ctx != NULL) {
		for (int i=0 ; i<((DPGraph*)ctx)->numOfInsts ; i++ ) {
			free(((DPGraph*)ctx)->cmd_pArray[i].info); // freeing allocated memory for comand's info
		}
		free(((DPGraph*)ctx)->cmd_pArray);
		free(ctx);
	}
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
	// validating input
	if (ctx == NULL || theInst >= ((DPGraph*)ctx)->numOfInsts) {
		return -1;
	}
    return (((DPGraph*)ctx)->cmd_pArray[theInst].depth);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
	// validating input
	if (ctx == NULL || theInst >= ((DPGraph*)ctx)->numOfInsts)
		return -1;
	// it is not possible for both dp1 & dp2 to point at NULL
	if (((DPGraph*)ctx)->cmd_pArray[theInst].dp1 == NULL) {
		*src1DepInst = ENTRYID;
		*src2DepInst = ((DPGraph*)ctx)->cmd_pArray[theInst].dp2->id;
		return 0;
	}
	// in case of no dependencies
	else if (((DPGraph*)ctx)->cmd_pArray[theInst].dp1->id == ENTRYID) {
		*src1DepInst = ENTRYID;
		*src2DepInst = ENTRYID;
		return 0;
	}
	else {
		*src1DepInst = ((DPGraph*)ctx)->cmd_pArray[theInst].dp1->id;
		if (((DPGraph*)ctx)->cmd_pArray[theInst].dp2 == NULL)
			*src2DepInst = ENTRYID;
		else
			*src2DepInst = ((DPGraph*)ctx)->cmd_pArray[theInst].dp2->id;
		return 0;
	}
    return -1;
}

int getProgDepth(ProgCtx ctx) {
	if (ctx == NULL)
		return 0;
	else
		return ((DPGraph*)ctx)->depth;
}


