//
//  state.h
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#ifndef PIC16F84A_emulator_state_h
#define PIC16F84A_emulator_state_h

#include <stdint.h>

uint16_t PC;
uint16_t stack[8];
uint16_t prog_mem[1094];
uint8_t ram[256];
uint8_t W;
uint16_t *stack_pointer;
uint64_t cycle_counter;

typedef struct {
	uint8_t C : 1;
	uint8_t DC : 1;
	uint8_t Z : 1;
	uint8_t PD : 1;
	uint8_t TO : 1;
	uint8_t RP0 : 1;
	uint8_t unused_RP1 : 1;
	uint8_t unused_IRP : 1;
} status_struct;

typedef union {
	uint8_t *address;
	status_struct *status;
} status_union;

status_struct *status;

#endif
