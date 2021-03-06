//
//  load.h
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#ifndef PIC16F84A_emulator_load_h
#define PIC16F84A_emulator_load_h

void load_hex(const char *path);
void load_map(const char *path);
void load_lst(const char *path);
void load_files(const char *dir);

#endif
