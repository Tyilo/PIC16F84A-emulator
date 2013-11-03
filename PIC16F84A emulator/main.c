//
//  main.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 01/11/2013.
//  Copyright (c) 2013 Tyilo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define INDF_ADDRESS   0x0
#define TMR0_ADDRESS   0x1
#define PCL_ADDRESS    0x2
#define STATUS_ADDRESS 0x3
#define FSR_ADDRESS    0x4
#define PORTA_ADDRESS  0x5
#define PORTB_ADDRESS  0x6

uint16_t PC;
uint16_t stack[8];
uint16_t prog_mem[1024];
uint8_t ram[256];
uint8_t W;
uint16_t *stack_pointer;

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

uint8_t get_real_address(uint8_t address) {
	if(address == INDF_ADDRESS) {
		address = ram[FSR_ADDRESS];
	}
	if(status->RP0) {
		if(address != INDF_ADDRESS && address != PCL_ADDRESS && address != STATUS_ADDRESS && address != FSR_ADDRESS) {
			address += 0x80;
		}
	}
	return address;
}

uint8_t read_ram(uint8_t address) {
	return ram[get_real_address(address)];
}

void write_ram(uint8_t address, uint8_t value) {
	ram[get_real_address(address)] = value;
}

void addlw(instruction ins) {
	uint8_t k = ins.vars_LW.k;
	uint8_t value = k + W;
	
	status->C = value < W;
	status->DC = (value & 0xF) < (W & 0xF);
	status->Z = value == 0;
	
	W = value;
}

void addwf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
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

void andlw(instruction ins) {
	uint8_t k = ins.vars_LW.k;
	uint8_t value = k & W;
	
	status->Z = value == 0;
	
	W = value;
}

void andwf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = read_ram(f) & W;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void iorlw(instruction ins) {
	uint8_t k = ins.vars_LW.k;
	uint8_t value = k | W;
	
	status->Z = value == 0;
	
	W = value;
}

void iorwf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = read_ram(f) | W;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void sublw(instruction ins) {
	uint8_t k = ins.vars_LW.k;
	uint8_t value = k - W;
	
	status->C = value > k;
	status->DC = (value & 0xF) > (k & 0xF);
	status->Z = value == 0;
	
	W = value;
}

void subwf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
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

void xorlw(instruction ins) {
	uint8_t k = ins.vars_LW.k;
	uint8_t value = W ^ k;
	
	status->Z = value == 0;
	
	W = value;
}

void xorwf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = W ^ read_ram(f);
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void swapf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t f_value = read_ram(f);
	uint8_t value = (f_value << 4) | (f_value >> 4);
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void movf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = read_ram(f);
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void movlw(instruction ins) {
	uint8_t k = ins.vars_LW.k;
	
	W = k;
}

void movwf(instruction ins) {
	uint8_t f = ins.vars_WF.f;
	
	write_ram(f, W);
}

void clrf(instruction ins) {
	uint8_t f = ins.vars_WF.f;
	
	status->Z = 1;
	
	write_ram(f, 0);
}

void clrw(instruction ins) {
	status->Z = 1;
	
	W = 0;
}

void comf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = read_ram(f) ^ 0xFF;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void decf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = read_ram(f) - 1;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void decfsz(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = read_ram(f) - 1;
	
	if(value == 0) {
		PC++;
	}
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void incf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = read_ram(f) + 1;
	
	status->Z = value == 0;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void incfsz(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t value = read_ram(f) + 1;
	
	if(value == 0) {
		PC++;
	}
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void rlf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t f_value = read_ram(f);
	uint8_t value = (f_value << 1) | status->C;
	
	status->C = f_value & 0x80;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void rrf(instruction ins) {
	uint8_t d = ins.vars_WF.d;
	uint8_t f = ins.vars_WF.f;
	
	uint8_t f_value = read_ram(f);
	uint8_t value = (f_value >> 1) | (status->C << 0x7);
	
	status->C = f_value & 0x1;
	
	if(d == 0) {
		W = value;
	} else {
		write_ram(f, value);
	}
}

void bcf(instruction ins) {
	uint8_t b = ins.vars_B.b;
	uint8_t f = ins.vars_B.f;
	
	uint8_t value = read_ram(f) & ((0x1 << b) ^ 0xFF);
	write_ram(f, value);
}

void bsf(instruction ins) {
	uint8_t b = ins.vars_B.b;
	uint8_t f = ins.vars_B.f;
	
	uint8_t value = read_ram(f) | (0x1 << b);
	write_ram(f, value);
}

void btfsc(instruction ins) {
	uint8_t b = ins.vars_B.b;
	uint8_t f = ins.vars_B.f;
	
	uint8_t value = (read_ram(f) & (0x1 << b)) >> b;
	if(value == 0) {
		PC++;
	}
}

void btfss(instruction ins) {
	uint8_t b = ins.vars_B.b;
	uint8_t f = ins.vars_B.f;
	
	uint8_t value = (read_ram(f) & (0x1 << b)) >> b;
	if(value == 1) {
		PC++;
	}
}

void nop(instruction ins) {
	// Noppity nop nop
}

void _goto(instruction ins) {
	uint8_t k = ins.vars_GOTO_CALL.k;
	PC = k - 1;
}

void call(instruction ins) {
	uint8_t k = ins.vars_GOTO_CALL.k;
	stack_pointer--;
	*stack_pointer = PC + 1;
	PC = k;
}

void _return(instruction ins) {
	// -1 Required because of implementation
	PC = *stack_pointer - 1;
	stack_pointer++;
}

void retlw(instruction ins) {
	uint8_t k = ins.vars_LW.k;
	
	W = k;
	_return(ins);
}

typedef struct {
	char *prefix;
	void (*implementation)(instruction);
} instruction_def;

instruction_def instructions[] = {
	{"11111", addlw},
	{"000111", addwf},
	{"111001", andlw},
	{"000101", andwf},
	{"111000", iorlw},
	{"000100", iorwf},
	{"11110", sublw},
	{"000010", subwf},
	{"111010", xorlw},
	{"000110", xorwf},
	{"001110", swapf},
	{"001000", movf},
	{"1100", movlw},
	{"0000001", movwf},
	{"0000011", clrf},
	{"0000010", clrw},
	{"001001", comf},
	{"000011", decf},
	{"001011", decfsz},
	{"001010", incf},
	{"001111", incfsz},
	{"001101", rlf},
	{"001100", rrf},
	{"0100", bcf},
	{"0101", bsf},
	{"0110", btfsc},
	{"0111", btfss},
	{"00000000000000", nop},
	{"00000000100000", nop},
	{"00000001000000", nop},
	{"00000001100000", nop},
	{"101", _goto},
	{"100", call},
	{"00000000001000", _return},
	{"1101", retlw},
	// Not implemented:
	{"00000000001001", /* retfie */ nop},
	{"00000001100100", /* clrwdt */ nop},
	{"00000001100011", /* sleep */ nop}
};

void bp_handler(void) {
	if(W < ram[PORTB_ADDRESS]) {
		printf("W = 0x%x, PORTB = 0x%x\n", W, ram[PORTB_ADDRESS]);
	}
}

uint16_t breakpoints[] = {0xa8};

void run() {
	PC = 0;
	W = 0;
	memset(ram, 0, sizeof(ram));
	memset(stack, 0, sizeof(stack));
	stack_pointer = &stack[sizeof(stack) / sizeof(stack[0]) - 1];
	{
		status_union u = {.address = &ram[3]};
		status = u.status;
	}
	
	while(1) {
		for(int i = 0; i < sizeof(breakpoints) / sizeof(breakpoints[0]); i++) {
			if(breakpoints[i] == PC) {
				bp_handler();
				break;
			}
		}
		
		instruction ins = (instruction)prog_mem[PC];
		char ins_string[14];
		for(int i = 0; i < sizeof(ins_string); i++) {
			ins_string[i] = '0' + ((ins.opcode >> (13 - i)) & 0x1); //((ins.opcode & (0x1 << (13 - i))) >> (13 - i));
		}
		for(int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
			instruction_def def = instructions[i];
			if(strncmp(ins_string, def.prefix, strlen(def.prefix)) == 0) {
				def.implementation(ins);
				break;
			}
		}
		PC++;
	}
}

uint64_t atoi_n(char *a, int n) {
	uint64_t value = 0;
	
	for(int i = 0; i < n; i++) {
		unsigned int c = a[i];
		if(c >= 'a') {
			c -= 'a' - 10;
		} else if(c >= 'A') {
			c -= 'A' - 10;
		} else {
			c -= '0';
		}
		
		value = (value << 4) | c;
	}
	
	return value;
}

void load_hex(char *path) {
	memset(prog_mem, 0, sizeof(prog_mem));
	
	FILE *f = fopen(path, "r");
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while((linelen = getline(&line, &linecap, f)) > 0) {
		if(linelen < 11) {
			continue;
		}
		if(line[0] != ':') {
			continue;
		}
		uint64_t data_len = atoi_n(&line[1], 2);
		uint64_t address = atoi_n(&line[3], 4);
		uint64_t type = atoi_n(&line[7], 2);
		if(type != 0) {
			continue;
		}
		
		for(int i = 0; i < data_len / 2; i++) {
			prog_mem[address / 2 + i] = __builtin_bswap16(atoi_n(&line[9 + i * 4], 4));
		}
	}
	
	fclose(f);
}

int main(int argc, const char *argv[]) {
	load_hex("/Users/Tyilo/Dropbox/Groups/EL - Asger & Jakob/MPLABXProjects/dot matrix display.X/dist/default/production/dot_matrix_display.X.production.hex");
	
	run();
	
	return 0;
}
