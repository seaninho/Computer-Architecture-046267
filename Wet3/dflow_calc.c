/* 046267 Computer Architecture - Spring 2017 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dflow_calc.h"

#define ENTRYID -1

typedef struct cmd_node cmdNode;
typedef struct dp_graph *DPGraph;

struct cmd_node{
	int id;
	InstInfo* info;			// command information (opcode, dstIdx, src1Idx, src2Idx)
	unsigned int duration;		// measured in clock cycles
	unsigned int depth;
	cmdNode* dp1;
	cmdNode* dp2;
};

struct dp_graph {
	cmdNode entry;
	cmdNode* cmd_pArray;
	unsigned int numOfInsts;
	unsigned int depth;
};

ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[], unsigned int numOfInsts) {
	if (!opsLatency || !progTrace || numOfInsts==0) {
		return PROG_CTX_NULL;
	}
	DPGraph graph = malloc(sizeof(DPGraph));
	if (!graph) {
		return PROG_CTX_NULL;
	}
	graph->cmd_pArray = malloc(numOfInsts*sizeof(cmdNode));
	if (!(graph->cmd_pArray)) {
		free(graph);
		return PROG_CTX_NULL;
	}
	graph->numOfInsts = numOfInsts;

	graph->entry->depth = 0;
	graph->entry->duration = 0;
	graph->entry->dp1 = NULL;
	graph->entry->dp2 = NULL;
	graph->entry->depth = 0;
	graph->entry->id = ENTRYID;
	graph->entry->info = NULL;

	for (int i = 0 ; i < numOfInsts ; i++) {
		graph->cmd_pArray[i].id = i;
		graph->cmd_pArray[i].info = malloc(sizeof(InstInfo*));
		graph->cmd_pArray[i].info->dstIdx = progTrace[i].dstIdx;
		graph->cmd_pArray[i].info->opcode = progTrace[i].opcode;
		graph->cmd_pArray[i].info->src1Idx = progTrace[i].src1Idx;
		graph->cmd_pArray[i].info->src2Idx = progTrace[i].src2Idx;
		graph->cmd_pArray[i].duration = opsLatency[progTrace[i].opcode];

		bool dp1_taken = false;
		graph->cmd_pArray[i].dp2 = NULL;
		for (int j = i-1 ; j >= 0; j--) {
			if(graph->cmd_pArray[i].info->src1Idx == graph->cmd_pArray[j].info->dstIdx ||
							graph->cmd_pArray[i].info->src2Idx == graph->cmd_pArray[j].info->dstIdx) {
				if (!dp1_taken) {
					graph->cmd_pArray[i].dp1 = &(graph->cmd_pArray[j]);
					dp1_taken = true;
					graph->cmd_pArray[i].depth = graph->cmd_pArray[j]->depth+graph->cmd_pArray[j]->duration;
				} else {
					graph->cmd_pArray[i].dp2 = &(graph->cmd_pArray[j]);
					if ((graph->cmd_pArray[i].depth) < (graph->cmd_pArray[j]->depth+graph->cmd_pArray[j]->duration)) {
						graph->cmd_pArray[i].depth = graph->cmd_pArray[j]->depth+graph->cmd_pArray[j]->duration;
					}
					break;
				}
			}
		}
		if (!dp1_taken) {
			graph->cmd_pArray[i].dp1 = &(graph->entry);
			graph->cmd_pArray[i].dp2 = NULL;
			graph->cmd_pArray[i].depth = 0;
		}
	}

    return graph;
}

void freeProgCtx(ProgCtx ctx) {
	if (ctx != NULL) {
		for (int i=0 ; i<((DPGraph)ctx)->numOfInsts ; i++ ) {
			free(((DPGraph)ctx)->cmd_pArray[i].info);
		}
		free(((DPGraph)ctx)->cmd_pArray);
		free(ctx);
	}
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
	if (ctx == NULL || theInst >= ((DPGraph)ctx)->numOfInsts) {
		return -1;
	}
    return (((DPGraph)ctx)->cmd_pArray[theInst]->depth);
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
	if (ctx == NULL || theInst >= ((DPGraph)ctx)->numOfInsts)
		return -1;
	if (((DPGraph)ctx)->cmd_pArray[theInst]->dp1->id == ENTRYID) {
		*src1DepInst = ENTRYID;
		*src2DepInst = ENTRYID;
		return 0;
	}
	else {
		*src1DepInst = ((DPGraph)ctx)->cmd_pArray[theInst]->dp1->id;
		if (((DPGraph)ctx)->cmd_pArray[theInst]->dp2 == NULL)
			*src2DepInst = ENTRYID;
		else
			*src2DepInst = ((DPGraph)ctx)->cmd_pArray[theInst]->dp2->id;;
		return 0;
	}
    return -1;
}

int getProgDepth(ProgCtx ctx) {
	if (ctx == NULL)
		return 0;
	else
		return ((DPGraph)ctx)->depth;
}


