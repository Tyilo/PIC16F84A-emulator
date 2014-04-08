//
//  symbols.h
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#ifndef PIC16F84A_emulator_symbols_h
#define PIC16F84A_emulator_symbols_h

#include <stdint.h>
#include "../uthash/src/uthash.h"

struct symbol {
    const char *name;
    uint16_t address;
    UT_hash_handle hh_name;
	UT_hash_handle hh_address;
};

struct symbol *prog_symbols_by_name;
struct symbol *prog_symbols_by_address;
struct symbol *ram_symbols_by_name;
struct symbol *ram_symbols_by_address;

const char *prog_name(uint16_t address);
uint16_t prog_address(char *str);
const char *ram_name(uint16_t address);
uint8_t ram_address(char *str);

#endif
