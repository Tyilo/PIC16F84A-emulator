//
//  utils.h
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#ifndef PIC16F84A_emulator_utils_h
#define PIC16F84A_emulator_utils_h

#include <stdint.h>

#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

uint8_t get_real_address(uint8_t address);
uint8_t read_ram(uint8_t address);
void write_ram(uint8_t address, uint8_t value);

#endif
