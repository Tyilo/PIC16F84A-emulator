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
    UT_hash_handle hh;
};

struct symbol *prog_symbols;
struct symbol *ram_symbols;

uint16_t prog_address(char *str);
uint8_t ram_address(char *str);

#endif
