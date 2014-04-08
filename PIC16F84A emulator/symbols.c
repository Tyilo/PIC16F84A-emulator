//
//  symbols.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include "symbols.h"
#include <assert.h>

const char *prog_name(uint16_t address) {
	struct symbol *s;
	HASH_FIND(hh_address, prog_symbols_by_address, &address, sizeof(address), s);
	if(!s) {
		return NULL;
	}
	return s->name;
}

uint16_t prog_address(char *str) {
	struct symbol *s;
	HASH_FIND(hh_name, prog_symbols_by_name, str, strlen(str), s);
	assert(s);
	return s->address;
}

const char *ram_name(uint16_t address) {
	struct symbol *s;
	HASH_FIND(hh_address, ram_symbols_by_address, &address, sizeof(address), s);
	if(!s) {
		return NULL;
	}
	return s->name;
}

uint8_t ram_address(char *str) {
	struct symbol *s;
	HASH_FIND(hh_name, ram_symbols_by_name, str, strlen(str), s);
	assert(s);
	return s->address;
}
