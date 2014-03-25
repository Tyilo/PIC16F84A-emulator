//
//  disassemble.h
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 25/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#ifndef PIC16F84A_emulator_disassemble_h
#define PIC16F84A_emulator_disassemble_h

#include "instructions.h"

instruction_def *ins_def_from_ins(instruction *ins);
char *disassemble_instruction(instruction *ins);
char *disassemble_program(void);

#endif
