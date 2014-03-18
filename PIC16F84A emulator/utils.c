//
//  utils.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include "utils.h"

#include "constants.h"
#include "state.h"

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
