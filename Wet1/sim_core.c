/* 046267 Computer Architecture - Spring 2017 - HW #1 */
/* This file should hold your implementation of the CPU pipeline core simulator */

#include "sim_api.h"

typedef enum {

    NO_HAZARD,						// No hazard has been detected, core may carry on

	EXE_SRC1_MEM_SRC2_DATA_HAZARD, 	// SIM_cmd in EXE pipeStage writes to src1 reg of SIM_cmd in ID pipeStage
								    // SIM_cmd in MEM pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	EXE_SRC1_WB_SRC2_DATA_HAZARD,  	// SIM_cmd in EXE pipeStage writes to src1 reg of SIM_cmd in ID pipeStage
								    // SIM_cmd in WB pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	EXE_SRC1_DATA_HAZARD, 		  	// SIM_cmd in EXE pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	MEM_SRC1_EXE_SRC2_DATA_HAZARD, 	// SIM_cmd in EXE pipeStage writes to src2 reg of SIM_cmd in ID pipeStage
								    // SIM_cmd in MEM pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	MEM_SRC1_WB_SRC2_DATA_HAZARD,  	// SIM_cmd in MEM pipeStage writes to src1 reg of SIM_cmd in ID pipeStage
	   	   	   	   	   	   	   	    // SIM_cmd in WB pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	MEM_SRC1_DATA_HAZARD,		   	// SIM_cmd in MEM pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	WB_SRC1_EXE_SRC2_DATA_HAZARD,  	// SIM_cmd in EXE pipeStage writes to src2 reg of SIM_cmd in ID pipeStage
	   	   	   	   	   	   	   	   	// SIM_cmd in WB pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	WB_SRC1_MEM_SRC2_DATA_HAZARD,  	// SIM_cmd in MEM pipeStage writes to src2 reg of SIM_cmd in ID pipeStage
								   	// SIM_cmd in WB pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	WB_SRC1_DATA_HAZARD,		   	// SIM_cmd in WB pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	EXE_SRC2_DATA_HAZARD,		   	// SIM_cmd in EXE pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	MEM_SRC2_DATA_HAZARD,		   	// SIM_cmd in MEM pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	WB_SRC2_DATA_HAZARD,    	   	// SIM_cmd in WB pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	EXE_DST_MEM_SRC1_DATA_HAZARD,	// SIM_cmd in EXE pipeStage writes to dst reg of SIM_cmd in ID pipeStage
    								// SIM_cmd in MEM pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	EXE_DST_MEM_SRC2_DATA_HAZARD,	// SIM_cmd in EXE pipeStage writes to dst reg of SIM_cmd in ID pipeStage
    								// SIM_cmd in MEM pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	EXE_DST_DATA_HAZARD,			// CMD_LOAD in EXE pipeStage writes to dst reg of SIM_cmd in ID pipeStage

	MEM_DST_EXE_SRC1_DATA_HAZARD,	// SIM_cmd in EXE pipeStage writes to src1 reg of SIM_cmd in ID pipeStage
									// SIM_cmd in MEM pipeStage writes to dst reg of SIM_cmd in ID pipeStage

	MEM_DST_EXE_SRC2_DATA_HAZARD,	// SIM_cmd in EXE pipeStage writes to src2 reg of SIM_cmd in ID pipeStage
									// SIM_cmd in MEM pipeStage writes to dst reg of SIM_cmd in ID pipeStage

	MEM_DST_DATA_HAZARD,			// CMD_LOAD in MEM pipeStage writes to dst reg of SIM_cmd in ID pipeStage

	WB_DST_DATA_HAZARD,				// CMD_LOAD in WB pipeStage writes to dst reg of SIM_cmd in ID pipeStage

	LOAD_SRC1_MEM_SRC2_DATA_HAZARD,	// CMD_LOAD in EXE pipeStage writes to src1 reg of SIM_cmd in ID pipeStage
	   	   	   	   	   	   	   	   	// SIM_cmd in MEM pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	LOAD_SRC1_DATA_HAZARD,			// CMD_LOAD in EXE pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	LOAD_SRC2_MEM_SRC1_DATA_HAZARD,	// CMD_LOAD in EXE pipeStage writes to src2 reg of SIM_cmd in ID pipeStage
	   	   	   	   	   	   	   	   	// SIM_cmd in MEM pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	LOAD_SRC2_DATA_HAZARD,			// CMD_LOAD in EXE pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	LOAD_DST_MEM_SRC1_DATA_HAZARD,	// CMD_LOAD in EXE pipeStage writes to dst reg of SIM_cmd in ID pipeStage
									// SIM_cmd in MEM pipeStage writes to src1 reg of SIM_cmd in ID pipeStage

	LOAD_DST_MEM_SRC2_DATA_HAZARD,	// CMD_LOAD in EXE pipeStage writes to dst reg of SIM_cmd in ID pipeStage
   	   	   	   						// SIM_cmd in MEM pipeStage writes to src2 reg of SIM_cmd in ID pipeStage

	LOAD_DST_DATA_HAZARD,			// CMD_LOAD in EXE pipeStage writes to dst reg of SIM_cmd in ID pipeStage

	CONTROL_HAZARD 				   	// Branch CMD taken

} hazardType;

SIM_coreState mips;					// Simulation core - 5 stages pipeline

bool branchTaken;					// Branch resolution - taken or not
bool read_fail;						// Read from memory - failed or not(succeeded)
bool stall;							// Clock cycle - stall or not

bool src1_src2_eq;					// A flag that indicates the src1 reg of the Sim_cmd in ID pipeStage is equal to its src2 reg
bool src1_dst_eq;					// A flag that indicates the src1 reg of the Sim_cmd in ID pipeStage is equal to its dst reg
bool src2_dst_eq;					// A flag that indicates the src2 reg of the Sim_cmd in ID pipeStage is equal to its dst reg

int	counter;						// Number of clock cycles to stall

int32_t val1;
int32_t val2;
int32_t dst_val;

int32_t result_ex;					// the ALU's result
int32_t result_mem;					// the memory reading result
int32_t result_wb;					// the result we need to write back to the register file
int32_t read_addr;					// read address sent to the sim_mem function
hazardType hazard_type;				// type of hazard being detected

/******* Static functions *******/

/* dstAffectingCMDs: determines whether the command write to the dst reg or not */
static bool dstAffectingCMDs(SIM_cmd_opcode cmd) {
	return (cmd == CMD_ADD || cmd == CMD_ADDI || cmd == CMD_SUB || cmd == CMD_SUBI ||
			cmd == CMD_LOAD);
}

/* dstAffectedCMDs: determines whether the command reads from the dst reg or not */
static bool dstAffectedCMDs(SIM_cmd_opcode cmd) {
	return (cmd == CMD_BR || cmd == CMD_BREQ || cmd == CMD_BRNEQ || cmd == CMD_STORE);
}

/* advancePC: advances the program counter by 4 in order to fetch new instruction */
static void advancePC() {
	mips.pc += 4;
}

/* clearPipeStage: clears a specific pipe stage, meaning resetting all values to 0 */
static void clearPipeStage(int i) {
	mips.pipeStageState[i].cmd.opcode = CMD_NOP;
	mips.pipeStageState[i].cmd.src1 = 0;
	mips.pipeStageState[i].cmd.src2 = 0;
	mips.pipeStageState[i].cmd.isSrc2Imm = false;
	mips.pipeStageState[i].cmd.dst = 0;
	mips.pipeStageState[i].src1Val = 0;
	mips.pipeStageState[i].src2Val = 0;
}

/* flushPipe: clears the whole pipe */
static void flushPipe() {
	for (int i = 0; i < SIM_PIPELINE_DEPTH; ++i) {
		clearPipeStage(i);
	}
}
/* hazardDetectionUnit: detects hazard such as data hazard, control hazard,
 * 						load hazard (which is a specific kind of data hazard)*/
static hazardType hazardDetectionUnit(bool branchTaken) {
	// branch is taken so we have to flush the first 3 stages of the pipe
	if (branchTaken) {
		return CONTROL_HAZARD;
	}

	SIM_cmd id_CMD = mips.pipeStageState[DECODE].cmd; // ID pipeStage command
	SIM_cmd exe_CMD = mips.pipeStageState[EXECUTE].cmd; // EXE pipeStage command
	SIM_cmd mem_CMD = mips.pipeStageState[MEMORY].cmd; // MEM pipeStage command
	SIM_cmd wb_CMD = mips.pipeStageState[WRITEBACK].cmd; // WB pipeStage command

	// we handle load SIM_cmd in exe pipeStage in a certain manner
	// there is a need to determine which id pipeStage register is the load SIM_cmd dst reg
	if (exe_CMD.opcode == CMD_LOAD) {

		if (exe_CMD.dst == id_CMD.src1) {
			if (id_CMD.isSrc2Imm == false) {
				if (id_CMD.src2 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode))
					return LOAD_SRC1_MEM_SRC2_DATA_HAZARD;
			}
			return LOAD_SRC1_DATA_HAZARD;
		}
		// if src2 is immediate number we don't need to compare it
		if (id_CMD.isSrc2Imm == false) {
			if (exe_CMD.dst == id_CMD.src2) {
				if (id_CMD.src1 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode))
					return LOAD_SRC2_MEM_SRC1_DATA_HAZARD;
				return LOAD_SRC2_DATA_HAZARD;
			}
		}

		if (exe_CMD.dst == id_CMD.dst) {
			if (id_CMD.src1 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode)) {
				return LOAD_DST_MEM_SRC1_DATA_HAZARD;
			}
			if (id_CMD.isSrc2Imm == false) {
				if (id_CMD.src2 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode))
					return LOAD_DST_MEM_SRC2_DATA_HAZARD;
			}
			return LOAD_DST_DATA_HAZARD;
		}
	}

	// we handle destination affecting SIM_cmd in a certain manner
	if (dstAffectedCMDs(id_CMD.opcode)) {
		// we start by checking the SIM_cmd in the exe pipeStage
		if (id_CMD.dst == exe_CMD.dst && dstAffectingCMDs(exe_CMD.opcode)) {
			if(id_CMD.opcode != CMD_BR) {
				// if the id pipeStage SIM_cmd has equal src1 reg and dst reg we need to raise the flag
				if (id_CMD.src1 == id_CMD.dst) {
					src1_dst_eq = true;
				}
				else {
					src1_dst_eq = false;

				}

				if (id_CMD.isSrc2Imm == false) {
					if (id_CMD.src2 == id_CMD.dst) {
							src2_dst_eq = true;
					}
					else {
						src2_dst_eq = false;
					}

					if (id_CMD.src2 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode)) {
						if (id_CMD.src1 == id_CMD.src2) {
							src1_src2_eq = true;
						}
						else {
							src1_src2_eq = false;
							return EXE_DST_MEM_SRC2_DATA_HAZARD;
						}
					}
				}

				if (id_CMD.src1 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode)) {
					return EXE_DST_MEM_SRC1_DATA_HAZARD;
				}

			}
			return EXE_DST_DATA_HAZARD;
		}
		// checking the SIM_cmd in the mem pipeStage
		if (id_CMD.dst == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode)) {
			if(id_CMD.opcode != CMD_BR) {
				if (id_CMD.src1 == id_CMD.dst) {
					src1_dst_eq = true;
				}
				else {
					src1_dst_eq = false;

				}
				if (id_CMD.isSrc2Imm == false) {
					if (id_CMD.src2 == id_CMD.dst) {
							src2_dst_eq = true;
					}
					else {
						src2_dst_eq = false;
					}

					if (id_CMD.src2 == exe_CMD.dst && dstAffectingCMDs(exe_CMD.opcode)) {
						if (id_CMD.src1 == id_CMD.src2) {
							src1_src2_eq = true;
						}
						else {
							src1_src2_eq = false;
							return MEM_DST_EXE_SRC2_DATA_HAZARD;
						}
					}
				}

				if (id_CMD.src1 == exe_CMD.dst && dstAffectingCMDs(exe_CMD.opcode)) {
					return MEM_DST_EXE_SRC1_DATA_HAZARD;
				}

			}
			return MEM_DST_DATA_HAZARD;
		}

		if (id_CMD.dst == wb_CMD.dst && dstAffectingCMDs(wb_CMD.opcode)) {
			return WB_DST_DATA_HAZARD;
		}
	}

	//each command other than branch, nop or halt has src1 register
	if (id_CMD.opcode != CMD_BR && id_CMD.opcode != CMD_NOP && id_CMD.opcode != CMD_HALT) {

		if (id_CMD.src1 == exe_CMD.dst && dstAffectingCMDs(exe_CMD.opcode)) {
			// if src2 reg is immediate, comparison is not necessary
			if (id_CMD.isSrc2Imm == false) {
				if (id_CMD.src2 == id_CMD.src1) {
					src1_src2_eq = true;
				}
				else {
					src1_src2_eq = false;
					if (id_CMD.src2 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode))
						return EXE_SRC1_MEM_SRC2_DATA_HAZARD;
					if (id_CMD.src2 == wb_CMD.dst && dstAffectingCMDs(wb_CMD.opcode))
						return EXE_SRC1_WB_SRC2_DATA_HAZARD;
				}
			}
			return EXE_SRC1_DATA_HAZARD;
		}


		if (id_CMD.src1 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode)) {
			// if src2 reg is immediate, comparison is not necessary
			if (id_CMD.isSrc2Imm == false) {
				if (id_CMD.src2 == id_CMD.src1) {
					src1_src2_eq = true;
				}
				else {
					src1_src2_eq = false;
					if (id_CMD.src2 == exe_CMD.dst && dstAffectingCMDs(exe_CMD.opcode))
						return MEM_SRC1_EXE_SRC2_DATA_HAZARD;
					if (id_CMD.src2 == wb_CMD.dst && dstAffectingCMDs(wb_CMD.opcode))
						return MEM_SRC1_WB_SRC2_DATA_HAZARD;
				}
			}
			return MEM_SRC1_DATA_HAZARD;
		}


		if (id_CMD.src1 == wb_CMD.dst && dstAffectingCMDs(wb_CMD.opcode)) {
			// if src2 reg is not immediate, comparison is necessary
			if (id_CMD.isSrc2Imm == false) {
				if (id_CMD.src2 == exe_CMD.dst && dstAffectingCMDs(exe_CMD.opcode))
					return WB_SRC1_EXE_SRC2_DATA_HAZARD;
				if (id_CMD.src2 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode))
					return WB_SRC1_MEM_SRC2_DATA_HAZARD;
			}
			return WB_SRC1_DATA_HAZARD;
		}

		// checking if the command has a src2 register
		if (id_CMD.isSrc2Imm == false) {
			if (id_CMD.src2 == exe_CMD.dst && dstAffectingCMDs(exe_CMD.opcode))
				return EXE_SRC2_DATA_HAZARD;
			if (id_CMD.src2 == mem_CMD.dst && dstAffectingCMDs(mem_CMD.opcode))
				return MEM_SRC2_DATA_HAZARD;
			if (id_CMD.src2 == wb_CMD.dst && dstAffectingCMDs(wb_CMD.opcode))
				return WB_SRC2_DATA_HAZARD;
		}
		return NO_HAZARD;
	}
	return NO_HAZARD;
}



/*! SIM_CoreReset: Reset the processor core simulator machine to start new simulation
  Use this API to initialize the processor core simulator's data structures.
  The simulator machine must complete this call with these requirements met:
  - PC = 0  (entry point for a program is at address 0)
  - All the register file is cleared (all registers hold 0)
  - The value of IF is the instuction in address 0x0
  \returns 0 on success. <0 in case of initialization failure.
*/
int SIM_CoreReset(void) {
	// reseting all necessary flags & global variables
	branchTaken = false;
	val2 = 0;
	val1 = 0;
	dst_val = 0;
	result_ex = 0;
	result_mem = 0;
	result_wb = 0;
	hazard_type = NO_HAZARD;
	read_fail = false;
	stall = false;
	src1_src2_eq = false;
	src1_dst_eq = false;
	src2_dst_eq = false;
	counter = 0;
	// reseting the simulation core
	mips.pc = 0;
	memset(mips.regFile, 0, SIM_REGFILE_SIZE);
	flushPipe();
	// fetching the 1st instruction into the IF pipeStage
	SIM_MemInstRead(mips.pc, &mips.pipeStageState[FETCH].cmd);
	return 0;
}

/*! SIM_CoreClkTick: Update the core simulator's state given one clock cycle.
  This function is expected to update the core pipeline given a clock cycle event.
*/
void SIM_CoreClkTick() {

/****WB Stage****/

	if (hazard_type == CONTROL_HAZARD) {
		branchTaken = false;
	}

	if (!split_regfile){
		SIM_cmd_opcode wb_opcode = mips.pipeStageState[WRITEBACK].cmd.opcode;
		if (wb_opcode == CMD_ADD || wb_opcode == CMD_SUB || wb_opcode == CMD_ADDI
				|| wb_opcode == CMD_SUBI || wb_opcode == CMD_LOAD)	{
			mips.regFile[mips.pipeStageState[WRITEBACK].cmd.dst] = result_wb;
		}
	}

	if (!read_fail)	{
		mips.pipeStageState[WRITEBACK] = mips.pipeStageState[MEMORY];
		result_wb = result_mem;
		if (split_regfile){
			SIM_cmd_opcode wb_opcode = mips.pipeStageState[WRITEBACK].cmd.opcode;
			if (wb_opcode == CMD_ADD || wb_opcode == CMD_SUB || wb_opcode == CMD_ADDI
					|| wb_opcode == CMD_SUBI || wb_opcode == CMD_LOAD)	{
				mips.regFile[mips.pipeStageState[WRITEBACK].cmd.dst] = result_wb;
			}
		}

	}

	else {
		clearPipeStage(WRITEBACK);
	}

/****end WB Stage****/


/****MEM Stage****/

	bool temp_read_fail = false;

	if (hazard_type == CONTROL_HAZARD) {
		clearPipeStage(MEMORY);
	}

	else {
		if (!read_fail)	{
			mips.pipeStageState[MEMORY] = mips.pipeStageState[EXECUTE];
			read_addr = result_ex;
		}

		switch (mips.pipeStageState[MEMORY].cmd.opcode)	{

			case CMD_LOAD:
				if (SIM_MemDataRead(read_addr, &result_mem) == 0)	{
					temp_read_fail = false;
				} else {
					temp_read_fail = true;
				}
				break;

			case CMD_STORE:
				result_mem = mips.regFile[mips.pipeStageState[MEMORY].cmd.src1];
				SIM_MemDataWrite(result_ex, result_mem);
				break;

			case CMD_BR:
				branchTaken = true;
				result_mem = result_ex;
				break;

			case CMD_BREQ:
				if (mips.pipeStageState[MEMORY].src1Val == mips.pipeStageState[MEMORY].src2Val) {
					branchTaken = true;
					result_mem = result_ex;
				}
				break;

			case CMD_BRNEQ:
				if (mips.pipeStageState[MEMORY].src1Val != mips.pipeStageState[MEMORY].src2Val) {
					branchTaken = true;
					result_mem = result_ex;
				}
				break;

			default:
				result_mem = result_ex;
		}
	}

/****end MEM Stage****/


/****EXE Stage****/

	if (hazard_type == CONTROL_HAZARD) {
		clearPipeStage(EXECUTE);
	}

	else if (!read_fail) {
		// if stall flag is raised, it means we need to create a delay in the EXE pipeStage
		// and decrease the counter by 1
		if (stall) {
			clearPipeStage(EXECUTE);
		}
		//
		else {
			// in case forwarding flag is activated and a hazard was detected

			if (forwarding && (hazard_type != NO_HAZARD)) {

				if (hazard_type == EXE_SRC1_MEM_SRC2_DATA_HAZARD || hazard_type == EXE_SRC1_WB_SRC2_DATA_HAZARD ||
					hazard_type == EXE_SRC1_DATA_HAZARD || hazard_type == MEM_DST_EXE_SRC1_DATA_HAZARD) {

						val1 = result_mem;
				}

				if (hazard_type == EXE_SRC2_DATA_HAZARD || hazard_type == MEM_SRC1_EXE_SRC2_DATA_HAZARD ||
					hazard_type == WB_SRC1_EXE_SRC2_DATA_HAZARD || hazard_type == MEM_DST_EXE_SRC2_DATA_HAZARD) {

						val2 = result_mem;
				}

				if (hazard_type == EXE_DST_MEM_SRC1_DATA_HAZARD || hazard_type == EXE_DST_MEM_SRC2_DATA_HAZARD ||
					hazard_type == EXE_DST_DATA_HAZARD) {

					 	dst_val = result_mem;
				}

				if (hazard_type == MEM_SRC1_EXE_SRC2_DATA_HAZARD || hazard_type == MEM_SRC1_WB_SRC2_DATA_HAZARD ||
					hazard_type == MEM_SRC1_DATA_HAZARD || hazard_type == LOAD_SRC1_MEM_SRC2_DATA_HAZARD ||
					hazard_type == EXE_DST_MEM_SRC1_DATA_HAZARD || hazard_type == LOAD_SRC1_DATA_HAZARD) {

					 	val1 = result_wb;
				}

				if (hazard_type == EXE_SRC1_MEM_SRC2_DATA_HAZARD || hazard_type == WB_SRC1_MEM_SRC2_DATA_HAZARD ||
					hazard_type == MEM_SRC2_DATA_HAZARD || hazard_type == LOAD_SRC2_MEM_SRC1_DATA_HAZARD ||
					hazard_type == EXE_DST_MEM_SRC2_DATA_HAZARD || hazard_type == LOAD_SRC2_DATA_HAZARD) {

						val2 = result_wb;
				}

				if (hazard_type == MEM_DST_EXE_SRC1_DATA_HAZARD || hazard_type == MEM_DST_EXE_SRC2_DATA_HAZARD ||
					hazard_type == MEM_DST_DATA_HAZARD || hazard_type == LOAD_DST_MEM_SRC1_DATA_HAZARD || hazard_type == LOAD_DST_MEM_SRC2_DATA_HAZARD) {

					 	dst_val = result_wb;
				}

				if (src1_src2_eq) {
					val2 = val1;
				}

				if (src1_dst_eq) {
					val1 = dst_val;
				}

				if (src2_dst_eq) {
					val2 = dst_val;
				}

				stall = false;
				hazard_type = NO_HAZARD;
				counter = 0;

				mips.pipeStageState[DECODE].src1Val = val1;
				mips.pipeStageState[DECODE].src2Val = val2;

			}

			mips.pipeStageState[EXECUTE] = mips.pipeStageState[DECODE];
			switch (mips.pipeStageState[EXECUTE].cmd.opcode) {
				case CMD_NOP:
					result_ex = 0;
					break;

				case CMD_ADD:
					result_ex = val1 + val2;
					break;

				case CMD_SUB:
					result_ex = val1 - val2;
					break;

				case CMD_ADDI:
					result_ex = val1 + val2;
					break;

				case CMD_SUBI:
					result_ex = val1 - val2;
					break;

				case CMD_LOAD:
					result_ex = val1 + val2;
					break;

				case CMD_STORE:
					result_ex = dst_val + val2;
					break;

				case CMD_BR:
					result_ex = mips.pc + dst_val;
					break;

				case CMD_BREQ:
					result_ex = mips.pc + dst_val;
					break;

				case CMD_BRNEQ:
					result_ex = mips.pc + dst_val;
					break;

				case CMD_HALT:
					result_ex = 0;
					break;

				default:
					result_ex = 0;
			}
		}
	} else {
		if (counter == 1 && !split_regfile) {
			counter--;
			stall = false;
		}
	}

/****end EXE Stage****/


/****ID Stage****/

	if (hazard_type == CONTROL_HAZARD) {
		clearPipeStage(DECODE);
	}
	else {
		if (!stall && !read_fail) {
			mips.pipeStageState[DECODE] = mips.pipeStageState[FETCH];
		}
		if (!(mips.pipeStageState[DECODE].cmd.isSrc2Imm))	{
			mips.pipeStageState[DECODE].src2Val =
				mips.regFile[(int)mips.pipeStageState[DECODE].cmd.src2];
		}else{
			mips.pipeStageState[DECODE].src2Val =
									mips.pipeStageState[DECODE].cmd.src2;
		}
		mips.pipeStageState[DECODE].src1Val =
			mips.regFile[mips.pipeStageState[DECODE].cmd.src1];

		val1 = mips.pipeStageState[DECODE].src1Val;
		val2 = mips.pipeStageState[DECODE].src2Val;
		dst_val = mips.regFile[mips.pipeStageState[DECODE].cmd.dst];
	}

/****end ID Stage****/


/****IF Stage****/

	if (!stall && !read_fail) {
		if (hazard_type != CONTROL_HAZARD) {
			advancePC();
			SIM_MemInstRead(mips.pc, &mips.pipeStageState[FETCH].cmd);
		}
		else {
			mips.pc = result_mem;
			SIM_MemInstRead(mips.pc, &mips.pipeStageState[FETCH].cmd);
			hazard_type = NO_HAZARD;
		}
	}

/****end IF Stage****/

	read_fail = temp_read_fail;

	if (stall) {
		counter--;
		if (counter == 0) {
			stall = false;
		}
		if (!forwarding) {
			hazard_type = NO_HAZARD;
		}
	}

	if (!read_fail)	{

		src1_src2_eq = false;
		src1_dst_eq = false;
		src2_dst_eq = false;

		hazard_type = hazardDetectionUnit(branchTaken);
		if (hazard_type != NO_HAZARD && hazard_type != CONTROL_HAZARD) {

			if (forwarding) {

				if (hazard_type == LOAD_SRC1_DATA_HAZARD || hazard_type == LOAD_SRC2_DATA_HAZARD ||
					hazard_type == LOAD_SRC1_MEM_SRC2_DATA_HAZARD || hazard_type == LOAD_SRC2_MEM_SRC1_DATA_HAZARD ||
					hazard_type == LOAD_DST_MEM_SRC1_DATA_HAZARD || hazard_type == LOAD_DST_MEM_SRC2_DATA_HAZARD ||
					hazard_type == LOAD_DST_DATA_HAZARD) {

					stall = true;
					counter = 1;
				}
			} else if (split_regfile) {

				if (hazard_type == EXE_SRC1_MEM_SRC2_DATA_HAZARD || hazard_type == EXE_SRC1_WB_SRC2_DATA_HAZARD ||
					hazard_type == EXE_SRC1_DATA_HAZARD || hazard_type == MEM_SRC1_EXE_SRC2_DATA_HAZARD ||
					hazard_type == WB_SRC1_EXE_SRC2_DATA_HAZARD || hazard_type == EXE_SRC2_DATA_HAZARD ||
					hazard_type == EXE_DST_MEM_SRC1_DATA_HAZARD || hazard_type == EXE_DST_MEM_SRC2_DATA_HAZARD ||
					hazard_type == MEM_DST_EXE_SRC1_DATA_HAZARD || hazard_type == MEM_DST_EXE_SRC2_DATA_HAZARD ||
					hazard_type == EXE_DST_DATA_HAZARD || hazard_type == LOAD_SRC1_MEM_SRC2_DATA_HAZARD ||
					hazard_type == LOAD_SRC1_DATA_HAZARD || hazard_type == LOAD_SRC2_MEM_SRC1_DATA_HAZARD ||
					hazard_type == LOAD_SRC2_DATA_HAZARD || hazard_type == LOAD_DST_MEM_SRC1_DATA_HAZARD ||
					hazard_type == LOAD_DST_MEM_SRC2_DATA_HAZARD || hazard_type == LOAD_DST_DATA_HAZARD) {

						stall = true;
						counter = 2;
				}

				else if (hazard_type == MEM_SRC1_WB_SRC2_DATA_HAZARD || hazard_type == MEM_SRC1_DATA_HAZARD	||
						hazard_type == WB_SRC1_MEM_SRC2_DATA_HAZARD	|| hazard_type == MEM_SRC2_DATA_HAZARD ||
						hazard_type == MEM_DST_DATA_HAZARD) {

						stall = true;
						counter = 1;
				}

				else {

					hazard_type = NO_HAZARD;
				}

			}

			else {

				stall = true;

				if (hazard_type == EXE_SRC1_MEM_SRC2_DATA_HAZARD ||	hazard_type == EXE_SRC1_WB_SRC2_DATA_HAZARD ||
					hazard_type == EXE_SRC1_DATA_HAZARD || hazard_type == MEM_SRC1_EXE_SRC2_DATA_HAZARD ||
					hazard_type == WB_SRC1_EXE_SRC2_DATA_HAZARD || hazard_type == EXE_SRC2_DATA_HAZARD ||
					hazard_type == EXE_DST_MEM_SRC1_DATA_HAZARD || hazard_type == EXE_DST_MEM_SRC2_DATA_HAZARD ||
					hazard_type == MEM_DST_EXE_SRC1_DATA_HAZARD || hazard_type == MEM_DST_EXE_SRC2_DATA_HAZARD ||
					hazard_type == EXE_DST_DATA_HAZARD || hazard_type == LOAD_SRC1_MEM_SRC2_DATA_HAZARD ||
					hazard_type == LOAD_SRC1_DATA_HAZARD || hazard_type == LOAD_SRC2_MEM_SRC1_DATA_HAZARD ||
					hazard_type == LOAD_SRC2_DATA_HAZARD || hazard_type == LOAD_DST_MEM_SRC1_DATA_HAZARD ||
					hazard_type == LOAD_DST_MEM_SRC2_DATA_HAZARD || hazard_type == LOAD_DST_DATA_HAZARD) {

					counter = 3;
				}

				else if (hazard_type == MEM_SRC1_WB_SRC2_DATA_HAZARD ||	hazard_type == MEM_SRC1_DATA_HAZARD	||
						hazard_type == WB_SRC1_MEM_SRC2_DATA_HAZARD	|| hazard_type == MEM_SRC2_DATA_HAZARD ||
						hazard_type == MEM_DST_DATA_HAZARD) {

					counter = 2;

				}

				else {

					counter = 1;
				}
			}
		}
	}
}

/*! SIM_CoreGetState: Return the current core (pipeline) internal state
    curState: The returned current pipeline state
    The function will return the state of the pipe at the end of a cycle
*/
void SIM_CoreGetState(SIM_coreState *curState) {
	*curState = mips;
}



