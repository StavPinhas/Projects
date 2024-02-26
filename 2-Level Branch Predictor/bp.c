/* 046267 Computer Architecture - HW #1                                 */
/* This file should hold your implementation of the predictor simulator */
	
	#include "bp_api.h"
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <math.h>
	#include <stdbool.h> 
	

	enum {SNT, WNT , WT, ST};

	
	
	//row in btb
	typedef struct {
		uint32_t tag;
		uint8_t* hist_table;
		uint32_t target_pc;
	} row;


	//btb parameters
	typedef struct {
		int btbSize;
		int tagSize;
		int histSize;
		bool isGlobalHist;
		bool isGlobalTable;
		int FSMstate;
		int shared;
	} btb_params;
	

	
	row* the_btb;
	btb_params params;
	SIM_stats results;
	uint8_t* state_machine_table;
	uint8_t* hist_reg;

	//aux functions
	int Get_FSM_Index(uint32_t pc, uint8_t hist_reg);
	int Get_Index (uint32_t pc);
	uint8_t reg_update(uint8_t curr_hist, bool taken);
	int new_state_update(int curr_state, bool taken);
	int Get_Tag(uint32_t pc);
	void initialize_BTB_parameters(unsigned btbSize, unsigned histSize, unsigned tagSize, unsigned fsmState,
                               bool isGlobalHist, bool isGlobalTable, int Shared);
	int allocate_BTB_table();
	int initialize_hist_regs();
	int initialize_state_machine_tables();
	bool Missed(uint32_t pc, int32_t targetPc, bool taken, uint32_t pred_dst);
	void Tag_Not_Exist_Handler(int index, uint32_t targetPc, int tag, uint8_t* hist_in_index );
	uint8_t get_predicted_state(uint32_t pc, uint8_t hist_in_index);
	bool is_prediction_correct(int tag, int index, uint8_t predicted_state);

	
	
	

	void initialize_BTB_parameters(unsigned btbSize, unsigned histSize, unsigned tagSize, unsigned fsmState,
                               bool isGlobalHist, bool isGlobalTable, int Shared) {
    //initialize parameters
    params.isGlobalHist = isGlobalHist;
	params.histSize = histSize;
    params.tagSize = tagSize;
    params.FSMstate = fsmState;
	params.btbSize = btbSize;
    params.shared = Shared;
    params.isGlobalTable = isGlobalTable; 
}

int allocate_BTB_table() {
    //btb allocate
    the_btb = (row *)malloc(sizeof(row) * params.btbSize);
    //malloc failed
    if (the_btb == NULL) {
        return -1;
    }
	//set initial values
    for (int i = 0; i < params.btbSize; i++) {
		the_btb[i].target_pc = 0;
        the_btb[i].tag = -1;
    }
    return 0;
}

int initialize_hist_regs() {
    //allocate memory for history register
    hist_reg = (uint8_t *)malloc(sizeof(uint8_t) * (params.isGlobalHist ? 1 : params.btbSize));
    //malloc failed
    if (hist_reg == NULL) {
        return -1;
    }
	//global history
    if (params.isGlobalHist) {
        *hist_reg = 0;
		//same history register
        for (int i = 0; i < params.btbSize; i++) {
            the_btb[i].hist_table = hist_reg;
        }
    } else {
        for (int i = 0; i < params.btbSize; i++) {
            hist_reg[i] = 0;
            the_btb[i].hist_table = &(hist_reg[i]);
        }
    }
    results.size += params.histSize * (params.isGlobalHist ? 1 : params.btbSize);
    return 0;
}

int initialize_state_machine_tables() {
    int size_of_fsm;
    size_of_fsm = params.isGlobalTable ? pow(2, params.histSize) : pow(2, params.histSize) * params.btbSize;
    state_machine_table = (uint8_t *)malloc(sizeof(uint8_t) * size_of_fsm);
    //malloc failed
    if (state_machine_table == NULL) {
        return -1;
    }
    for (int i = 0; i < size_of_fsm; i++) {
        state_machine_table[i] = params.FSMstate;
    }
    results.size += (params.isGlobalTable ? 2 : 2 * params.btbSize) * pow(2, params.histSize);
    return 0;
}

int new_state_update(int curr_state, bool taken) {
    // If the branch is taken (or not taken) and the current state is less than ST (or more than SNT),
    if ((taken && curr_state < ST) || (!taken && curr_state > SNT)) {
		//returning the next state
        return curr_state + (taken ? 1 : -1);
    }
    // If none of the conditions are met, return the current state
    return curr_state;
}
	


	uint8_t reg_update(uint8_t curr_hist, bool taken) {
    // shift left by 1 bit to make room for the new branch outcome
    curr_hist <<= 1;

    // if the branch is taken, add 1 to the history register
    if (taken) {
        curr_hist += 1;
    }

    // ensuring the history register size is maintained
    return curr_hist % (1 << params.histSize);
}
	
	int BP_init(unsigned btbSize, unsigned histSize, unsigned tagSize, unsigned fsmState,
		bool isGlobalHist, bool isGlobalTable, int Shared) {
		//initialze btb parameters
		initialize_BTB_parameters(btbSize, histSize, tagSize, fsmState, isGlobalHist, isGlobalTable, Shared );
	
		//initialize stats
		results.size = params.btbSize*(params.tagSize + 31);
		results.br_num = 0;
		results.flush_num = 0;
		//initialize table
		int btb_initialize = allocate_BTB_table();
		if(btb_initialize == -1){
			return -1;
		}
	
		//initialize history register
		int initialize_hist_reg = initialize_hist_regs();
		if (initialize_hist_reg == -1) {
        	free(the_btb);
        	return -1;
   	    }


		//initialize fsm
		int fsm_initialize = initialize_state_machine_tables();
		if(fsm_initialize == -1){
			free(the_btb);
        	free(hist_reg);
			return -1;
		}

		return 0;
	}

	//return true if a flush is needed (miss) , false if predicted right
	bool Missed(uint32_t pc, int32_t targetPc, bool taken, uint32_t pred_dst){
		return (( (pred_dst != targetPc) && (taken) ) || ( (pred_dst!=pc+4) && (!taken) ));
	}
	
	

uint8_t get_predicted_state(uint32_t pc, uint8_t hist_in_index) {
    int fsm_index = Get_FSM_Index(pc, hist_in_index);
    return state_machine_table[fsm_index];
}

bool is_prediction_correct(int tag, int index, uint8_t predicted_state) {
    return (tag == the_btb[index].tag && (predicted_state > WNT));
}

	bool BP_predict(uint32_t pc, uint32_t *dst) {
    int tag = Get_Tag(pc);
    int index = Get_Index(pc);
    uint8_t hist_in_index = *the_btb[index].hist_table;
    uint8_t predicted_state = get_predicted_state(pc, hist_in_index);
    
    if (is_prediction_correct(tag, index, predicted_state)) {
        *dst = the_btb[index].target_pc;
        return true;
    }

    //not taken / not exist
    *dst = pc + 4;
    return false;
}

void Tag_Not_Exist_Handler(int index, uint32_t targetPc, int tag, uint8_t* hist_in_index ){
	the_btb[index].tag = tag;
	the_btb[index].target_pc = targetPc;
	int size_of_fsm = 1 << params.histSize;
	if (!params.isGlobalHist) 		
		*hist_in_index = 0;
	if (!params.isGlobalTable){	
		int offset = size_of_fsm*index;
		for (int i=0; i<size_of_fsm; i++){
			state_machine_table[offset+i] = params.FSMstate;
		}
	}	
}
	
	void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
		int index = Get_Index(pc);
		int tag = Get_Tag(pc);
		uint32_t exist_target_pc = the_btb[index].target_pc ;
		uint8_t hist_in_index = *(the_btb[index].hist_table);
		results.br_num++;
		//check if missed
		if (Missed(pc, targetPc, taken, pred_dst))
			//updating flush counter
			results.flush_num++;		
		//if tag doesn't exist in table
		if (tag != the_btb[index].tag) {    
			Tag_Not_Exist_Handler( index, targetPc, tag, &hist_in_index );
		}
		int fsm_index = Get_FSM_Index(pc, hist_in_index);	
		//target is different than the existing one
		if (exist_target_pc != targetPc)
			the_btb[index].target_pc = targetPc;
	
		*(the_btb[index].hist_table) = reg_update(hist_in_index, taken);
		state_machine_table[fsm_index] = new_state_update(state_machine_table[fsm_index],taken);
		return;
	}

	
	
	void BP_GetStats(SIM_stats *curStats){
		curStats->flush_num = results.flush_num;
		curStats->size = results.size;
		curStats->br_num = results.br_num;
		if(state_machine_table != NULL)
		free(state_machine_table);
		if(hist_reg != NULL)
		free(hist_reg);
		if(the_btb != NULL)
		free(the_btb);
		return;
	}
	
	int Get_Tag(uint32_t pc) { 
		int tag = pc >> (2 + (int)log2(params.btbSize));
		// create a mask with the appropriate number of bits set to 1
		int mask = (1 << params.tagSize) - 1; 
		//extracting the correct bits of the tag
		return tag & mask; 
	}

	int Get_Index(uint32_t pc){ 
		return (pc / 4) % params.btbSize;
	};
	
	
	
	
	int Get_FSM_Index(uint32_t pc, uint8_t hist_reg) {
		int fsm_index;
		int size_of_fsm = 1 << params.histSize;
		//local table
		if (!params.isGlobalTable) {							
			return (Get_Index(pc) * size_of_fsm + hist_reg);
		}
		//global table
		else{		
			if (params.shared == 0)		
				fsm_index=hist_reg;
			//lsb share
			else if (params.shared == 1) {	
				fsm_index = (pc >> 2) ^ hist_reg;
			}
			//mid share
			else {						
				fsm_index = (pc >> 16) ^ hist_reg;
			}
	
			return fsm_index % size_of_fsm;
		}
	}
	


	
