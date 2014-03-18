//
//  instructions.h
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#ifndef PIC16F84A_emulator_instructions_h
#define PIC16F84A_emulator_instructions_h

#include <stdint.h>

// These are in reverse order due to little endianness
typedef struct {
	uint8_t k;
	uint8_t padding;
} ins_vars_LW;

typedef struct {
	uint8_t f : 7;
	uint8_t d : 1;
	uint8_t padding;
} ins_vars_WF;

// __attribute__((packed)) required for sizeof(ins_vars_B) == 2
typedef struct __attribute__((packed)) {
	uint8_t f : 7;
	uint8_t b : 3;
	uint8_t padding : 6;
} ins_vars_B;

typedef struct {
	uint16_t k : 10;
	uint8_t padding : 6;
} ins_vars_GOTO_CALL;

typedef union {
	uint16_t opcode;
	ins_vars_LW vars_LW;
	ins_vars_WF vars_WF;
	ins_vars_B vars_B;
	ins_vars_GOTO_CALL vars_GOTO_CALL;
} instruction;

typedef struct {
	char *prefix;
	void (*implementation)(instruction);
} instruction_def;

extern instruction_def instructions[38];

#endif
