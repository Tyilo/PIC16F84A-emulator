//
//  tests.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include "tests.h"

#include <assert.h>
#include <stdio.h>

#include "instructions.h"
#include "state.h"
#include "symbols.h"
#include "utils.h"

void test_toggle_bit(void) {
	uint16_t toggle_bit = prog_address("toggle_bit");
	uint8_t bit_register = ram_address("bit_register");
	uint8_t bit_number = ram_address("bit_number");
	
	for(int i = 0; i < 256; i++) {
		for(int j = 0; j < 8; j++) {
			reset();
			
			PC = toggle_bit;
			
			ram[bit_register] = 0x40;
			ram[bit_number] = j;
			ram[ram[bit_register]] = i;
			
			run();
			
			assert(ram[ram[bit_register]] == (i ^ (1 << j)));
		}
	}
}

void test_bit_pattern(void) {
	uint16_t bit_pattern = prog_address("bit_pattern");
	
	for(int i = 0; i < 8; i++) {
		reset();
		PC = bit_pattern;
		W = i;
		
		run();
		
		assert(W == (1 << i));
	}
}

void test_delay(void) {
	uint16_t delay = prog_address("delay");
	
	instruction *ins = (instruction *)&prog_mem[delay + 1];
	
	uint8_t data[2][2];
	
	for(int j = 0; j < 2; j++) {
		ins->vars_LW.k = j + 1;
		for(int i = 0; i < 2; i++) {
			reset();
			
			PC = delay;
			W = i + 1;
			
			run();
			
			data[j][i] = cycle_counter;
		}
	}
	
	int diff1 = data[0][1] - data[0][0];
	int offset1 = data[0][0] - diff1;
	int diff2 = data[1][1] - data[1][0];
	int diff = diff2 - diff1;
	int offset2 = diff1 - diff;
	printf("%d + (%d + k) * W\n", offset1, offset2);
	
}

void (*tests[])(void) = {test_bit_pattern, test_toggle_bit, test_delay};

void test(void) {
	for(int i = 0; i < LENGTH(tests); i++) {
		tests[i]();
		
		reset();
		
		printf("Completed test %d sucessfully!\n", i + 1);
	}
}
