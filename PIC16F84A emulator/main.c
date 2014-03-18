//
//  main.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 01/11/2013.
//  Copyright (c) 2013 Tyilo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#include "load.h"
#include "runtime.h"
#include "tests.h"

int main(int argc, const char *argv[]) {
	
	load_files("/Users/Tyilo/Dropbox/Groups/EL/MPLABXProjects/RC.X");
	
	reset();
	test();
	
	return 0;
}
