//
//  main.c
//  hex_disasm
//
//  Created by Asger Hautop Drewsen on 25/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include <stdlib.h>
#include <stdio.h>
#include "load.h"
#include "disassemble.h"
#include <string.h>

int main(int argc, const char * argv[]) {
	if(argc != 2) {
		printf("Usage: hex_disasm hex_file\n");
		return 1;
	}
	
	const char *path = argv[1];
	
	const char *ext = strrchr(path, '.');
	
	if(strcmp(ext, ".hex") == 0) {
		load_hex(path);
	} else {
		load_files(path);
	}
	
	char *disassembly = disassemble_program();
	printf("%s", disassembly);
	
	free(disassembly);
	
    return 0;
}
