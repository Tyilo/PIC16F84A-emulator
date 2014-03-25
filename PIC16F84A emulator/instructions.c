//
//  instructions.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include "instructions.h"

#include <stdio.h>

#include "state.h"
#include "utils.h"

void addlw(instruction *ins) {
	uint8_t k = ins->vars_LW.k;
	uint8_t value = k + W;
	
	status->C = value < W;
	status->DC = (value & 0xF) < (W & 0xF);
	status->Z = value == 0;
	
	W = value;
}

void addwf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f) + W;
	
	status->C = value < W;
	status->DC = (value & 0xF) < (W & 0xF);
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void andlw(instruction *ins) {
	uint8_t k = ins->vars_LW.k;
	uint8_t value = k & W;
	
	status->Z = value == 0;
	
	W = value;
}

void andwf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f) & W;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void iorlw(instruction *ins) {
	uint8_t k = ins->vars_LW.k;
	uint8_t value = k | W;
	
	status->Z = value == 0;
	
	W = value;
}

void iorwf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f) | W;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void sublw(instruction *ins) {
	uint8_t k = ins->vars_LW.k;
	uint8_t value = k - W;
	
	status->C = value > k;
	status->DC = (value & 0xF) > (k & 0xF);
	status->Z = value == 0;
	
	W = value;
}

void subwf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t f_value = read_ram(f);
	uint8_t value = f_value - W;
	
	status->C = value > f_value;
	status->DC = (value & 0xF) > (f_value & 0xF);
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void xorlw(instruction *ins) {
	uint8_t k = ins->vars_LW.k;
	uint8_t value = W ^ k;
	
	status->Z = value == 0;
	
	W = value;
}

void xorwf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = W ^ read_ram(f);
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void swapf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t f_value = read_ram(f);
	uint8_t value = (f_value << 4) | (f_value >> 4);
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void movf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f);
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void movlw(instruction *ins) {
	uint8_t k = ins->vars_LW.k;
	
	W = k;
}

void movwf(instruction *ins) {
	uint8_t f = ins->vars_WF.f;
	
	write_ram(f, W);
}

void clrf(instruction *ins) {
	uint8_t f = ins->vars_WF.f;
	
	status->Z = 1;
	
	write_ram(f, 0);
}

void clrw(instruction *ins) {
	status->Z = 1;
	
	W = 0;
}

void comf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f) ^ 0xFF;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void decf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f) - 1;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void decfsz(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f) - 1;
	
	if(value == 0) {
		PC++;
		cycle_counter++;
	}
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void incf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f) + 1;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void incfsz(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t value = read_ram(f) + 1;
	
	if(value == 0) {
		PC++;
		cycle_counter++;
	}
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void rlf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t f_value = read_ram(f);
	uint8_t value = (f_value << 1) | status->C;
	
	status->C = f_value & 0x80;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void rrf(instruction *ins) {
	uint8_t d = ins->vars_WF.d;
	uint8_t f = ins->vars_WF.f;
	
	uint8_t f_value = read_ram(f);
	uint8_t value = (f_value >> 1) | (status->C << 0x7);
	
	status->C = f_value & 0x1;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void bcf(instruction *ins) {
	uint8_t b = ins->vars_B.b;
	uint8_t f = ins->vars_B.f;
	
	uint8_t value = read_ram(f) & ((0x1 << b) ^ 0xFF);
	write_ram(f, value);
}

void bsf(instruction *ins) {
	uint8_t b = ins->vars_B.b;
	uint8_t f = ins->vars_B.f;
	
	uint8_t value = read_ram(f) | (0x1 << b);
	write_ram(f, value);
}

void btfsc(instruction *ins) {
	uint8_t b = ins->vars_B.b;
	uint8_t f = ins->vars_B.f;
	
	uint8_t value = (read_ram(f) & (0x1 << b)) >> b;
	if(value == 0) {
		PC++;
		cycle_counter++;
	}
}

void btfss(instruction *ins) {
	uint8_t b = ins->vars_B.b;
	uint8_t f = ins->vars_B.f;
	
	uint8_t value = (read_ram(f) & (0x1 << b)) >> b;
	if(value == 1) {
		PC++;
		cycle_counter++;
	}
}

void nop(instruction *ins) {
	// Noppity nop nop
}

void _goto(instruction *ins) {
	uint16_t k = ins->vars_GOTO_CALL.k;
	PC = k - 1;
	cycle_counter++;
}

void call(instruction *ins) {
	uint16_t k = ins->vars_GOTO_CALL.k;
	stack_pointer--;
	*stack_pointer = PC + 1;
	PC = k;
	cycle_counter++;
}

void _return(instruction *ins) {
	// -1 Required because of implementation
	PC = *stack_pointer - 1;
	stack_pointer++;
	cycle_counter++;
}

void retlw(instruction *ins) {
	uint8_t k = ins->vars_LW.k;
	
	W = k;
	_return(ins);
}

void unimplemented(const char *name) {
	printf("WARNING: %s instruction is not implemented. Doing nothing!", name);
}

void retfie(instruction *ins) {
	unimplemented("retfie");
}

void clrwdt(instruction *ins) {
	unimplemented("clrwdt");
}

void _sleep(instruction *ins) {
	unimplemented("sleep");
}

instruction_def instructions[] = {
	{"11111", LW, addlw},
	{"000111", WF, addwf},
	{"111001", LW, andlw},
	{"000101", WF, andwf},
	{"111000", LW, iorlw},
	{"000100", WF, iorwf},
	{"11110", LW, sublw},
	{"000010", WF, subwf},
	{"111010", LW, xorlw},
	{"000110", WF, xorwf},
	{"001110", WF, swapf},
	{"001000", WF, movf},
	{"1100", LW, movlw},
	{"0000001", WF, movwf},
	{"0000011", WF, clrf},
	{"0000010", NO_ARGS, clrw},
	{"001001", WF, comf},
	{"000011", WF, decf},
	{"001011", WF, decfsz},
	{"001010", WF, incf},
	{"001111", WF, incfsz},
	{"001101", WF, rlf},
	{"001100", WF, rrf},
	{"0100", B, bcf},
	{"0101", B, bsf},
	{"0110", B, btfsc},
	{"0111", B, btfss},
	{"00000000000000", NO_ARGS, nop},
	{"00000000100000", NO_ARGS, nop},
	{"00000001000000", NO_ARGS, nop},
	{"00000001100000", NO_ARGS, nop},
	{"101", GOTO_CALL, _goto},
	{"100", GOTO_CALL, call},
	{"00000000001000", NO_ARGS, _return},
	{"1101", LW, retlw},
	// Not implemented:
	{"00000000001001", NO_ARGS, /* retfie */ retfie},
	{"00000001100100", NO_ARGS, /* clrwdt */ clrwdt},
	{"00000001100011", NO_ARGS, /* sleep */ _sleep}
};
