//
//  symbols.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include "symbols.h"
#include <assert.h>

uint16_t prog_address(char *str) {
	struct symbol *s;
	HASH_FIND_STR(prog_symbols, str, s);
	assert(s);
	return s->address;
}

uint8_t ram_address(char *str) {
	struct symbol *s;
	HASH_FIND_STR(ram_symbols, str, s);
	assert(s);
	return s->address;
}
