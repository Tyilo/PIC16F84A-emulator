//
//  runtime.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include "runtime.h"

#include <stdio.h>
#include <string.h>
#include "symbols.h"
#include "utils.h"
#include "state.h"
#include "constants.h"
#include "instructions.h"
#include "disassemble.h"

#include <dlfcn.h>
#include <assert.h>

char *breakpoints[] = {};
uint16_t breakpoint_addresses[LENGTH(breakpoints)];

void bp_handler(void) {
	printf("PC = 0x%x, W = 0x%x, PORTB = 0x%x\n", PC, W, ram[PORTB_ADDRESS]);
}

void initialize(void) {
	cycle_counter = 0;
	memset(stack, 0, sizeof(stack));
	stack_pointer = &stack[LENGTH(stack) - 1];
	{
		status_union u = {.address = &ram[STATUS_ADDRESS]};
		status = u.status;
	}
}

void reset(void) {
	PC = 0;
	W = 0;
	memset(ram, 0, sizeof(ram));
	initialize();
}

void run(void) {
	for(int i = 0; i < LENGTH(breakpoints); i++) {
		breakpoint_addresses[i] = prog_address(breakpoints[i]);
	}
	
	while(1) {
		for(int i = 0; i < LENGTH(breakpoint_addresses); i++) {
			if(breakpoint_addresses[i] == PC) {
				bp_handler();
				break;
			}
		}
		
		instruction *ins = (instruction *)&prog_mem[PC];
		char ins_string[14];
		for(int i = 0; i < sizeof(ins_string); i++) {
			ins_string[i] = '0' + ((ins->opcode >> (13 - i)) & 0x1); //((ins.opcode & (0x1 << (13 - i))) >> (13 - i));
		}
		
		instruction_def *def = ins_def_from_ins(ins);
		assert(def);
		
		def->implementation(ins);
		
		PC++;
		cycle_counter++;
		
		if(stack_pointer < stack) {
			return;
		}
		if(stack_pointer > &stack[LENGTH(stack) - 1]) {
			return;
		}
	}
}