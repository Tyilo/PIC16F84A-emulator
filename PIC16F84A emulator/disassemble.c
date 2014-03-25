//
//  disassemble.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 25/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include <dlfcn.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "disassemble.h"
#include "utils.h"
#include "state.h"

instruction_def *ins_def_from_ins(instruction *ins) {
	char ins_string[14];
	for(int i = 0; i < sizeof(ins_string); i++) {
		ins_string[i] = '0' + ((ins->opcode >> (13 - i)) & 0x1);
	}
	
	for(int i = 0; i < LENGTH(instructions); i++) {
		instruction_def *def = &instructions[i];
		if(strncmp(ins_string, def->prefix, strlen(def->prefix)) == 0) {
			return def;
		}
	}
	
	return NULL;
}

char *instruction_name(instruction_def *def) {
	Dl_info info;
	dladdr(def->implementation, &info);
	
	const char *name = info.dli_sname;
	
	if(name[0] == '_') {
		name = name + 1;
	}
	
	size_t name_len = strlen(name);
	char *upper_name = malloc(name_len + 1);
	upper_name[name_len] = '\0';
	
	for(int i = 0; i < name_len; i++) {
		upper_name[i] = toupper(name[i]);
	}
	
	return upper_name;
}

char *disassemble_instruction(instruction *ins) {
	instruction_def *def = ins_def_from_ins(ins);
	
	const char *name = instruction_name(def);
	
	const char *format;
	uint16_t arg1 = 0;
	uint16_t arg2 = 0;
	
	switch(def->type) {
		case LW:
			format = " 0x%X";
			arg1 = ins->vars_LW.k;
			break;
		case WF:
			if(strcmp(name, "CLRF") == 0 || strcmp(name, "MOVWF") == 0) {
				format = " 0x%X";
				arg1 = ins->vars_WF.f;
			} else {
				format = " 0x%X, %c";
				arg1 = ins->vars_WF.f;
				arg2 = (ins->vars_WF.d == 0)? 'W': 'F';
			}
			break;
		case B:
			format = " 0x%X, %d";
			arg1 = ins->vars_B.f;
			arg2 = ins->vars_B.b;
			break;
		case GOTO_CALL:
			format = " 0x%X";
			arg1 = ins->vars_GOTO_CALL.k;
			break;
		case NO_ARGS:
			format = "";
			break;
	}
	
	char *new_format;
	
	if(format[0] == '\0') {
		new_format = "%s";
	} else {
		size_t format_len = strlen(format);
		const char *format_prefix = "%-8s";
		size_t format_prefix_len = strlen(format_prefix);
		size_t total_len = format_prefix_len + format_len;
		new_format = malloc(total_len + 1);
		new_format[total_len] = '\0';
		memcpy(new_format, format_prefix, format_prefix_len);
		memcpy(new_format + format_prefix_len, format, format_len);
	}
	
	char *str;
	
	asprintf(&str, new_format, name, arg1, arg2);
	
	if(format[0] != '\0') {
		free(new_format);
	}
	
	return str;
}

char *disassemble_program(void) {
	char *disassembly = "";
	
	int i;
	for(i = LENGTH(prog_mem) - 1; i >= 0; i--) {
		instruction *ins = (instruction *)&prog_mem[i];
		instruction_def *def = ins_def_from_ins(ins);
		if(strcmp(instruction_name(def), "NOP") != 0) {
			break;
		}
	}
	
	int max_instruction_address = i;
	
	for(int i = 0; i < max_instruction_address; i++) {
		instruction *ins = (instruction *)&prog_mem[i];
		char *line = disassemble_instruction(ins);
		
		char *old_disassembly = disassembly;
		asprintf(&disassembly, "%s0x%03X  %s\n", old_disassembly, i, line);
		if(i > 0) {
			free(old_disassembly);
		}
		
		free(line);
	}
	
	char *old_disassembly = disassembly;
	asprintf(&disassembly, "%s; %lu nops following\n", old_disassembly, LENGTH(prog_mem) - max_instruction_address + 1);
	free(old_disassembly);
	
	return disassembly;
}
