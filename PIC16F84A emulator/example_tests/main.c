//
//  main.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 01/11/2013.
//  Copyright (c) 2013 Tyilo. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>

#include "load.h"
#include "runtime.h"
#include "tests.h"

int main(int argc, const char *argv[]) {
	char *project_path;
	asprintf(&project_path, "%s/../../example_project.X", dirname(__FILE__));
	
	load_files(project_path);
	
	free(project_path);
	
	reset();
	test();
	
	return 0;
}
