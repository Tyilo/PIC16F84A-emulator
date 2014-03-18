//
//  load.c
//  PIC16F84A emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#include "load.h"

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <libgen.h>

#include "state.h"
#include "utils.h"
#include "symbols.h"

uint64_t atoi_n(char *a, int n) {
	uint64_t value = 0;
	
	for(int i = 0; i < n; i++) {
		unsigned int c = a[i];
		if(c >= 'a') {
			c -= 'a' - 10;
		} else if(c >= 'A') {
			c -= 'A' - 10;
		} else {
			c -= '0';
		}
		
		value = (value << 4) | c;
	}
	
	return value;
}

void load_hex(char *path) {
	memset(prog_mem, 0, sizeof(prog_mem));
	
	FILE *f = fopen(path, "r");
	
	assert(f);
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while((linelen = getline(&line, &linecap, f)) > 0) {
		if(linelen < 11) {
			continue;
		}
		if(line[0] != ':') {
			continue;
		}
		uint64_t data_len = atoi_n(&line[1], 2);
		uint64_t address = atoi_n(&line[3], 4);
		uint64_t type = atoi_n(&line[7], 2);
		if(type != 0) {
			continue;
		}
		
		uint64_t max_address = (address + data_len) / 2;
		if(max_address > ONESIZE(prog_mem)) {
			printf("WARNING: Ignored hex field with address 0x%llx\n", max_address);
			continue;
		}
		
		for(int i = 0; i < data_len / 2; i++) {
			prog_mem[address / 2 + i] = __builtin_bswap16(atoi_n(&line[9 + i * 4], 4));
		}
	}
	
	fclose(f);
}

char *find_word(char **str) {
	char *token;
	
	while((token = strsep(str, " \n\t\r")) && token[0] == '\0') {
		
	}
	
	return token;
}

void load_map(char *path) {
	FILE *f = fopen(path, "r");
	
	assert(f);
	
	int header_lines_ago = -1;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while((linelen = getline(&line, &linecap, f)) > 0) {
		char *loc = strstr(line, "Symbols - Sorted by Address");
		
		if(loc) {
			header_lines_ago = 0;
			continue;
		}
		
		if(header_lines_ago == -1) {
			continue;
		}
		
		header_lines_ago++;
		
		if(header_lines_ago >= 3) {
			char *l_start = strdup(line);
			char *l = l_start;
			char *name = find_word(&l);
			
			if(!name) {
				free(l_start);
				break;
			}
			
			char *a = find_word(&l);
			
			uint16_t address = strtol(a, NULL, 0);
			
			struct symbol *s = malloc(sizeof(struct symbol));
			s->name = strdup(name);
			s->address = address;
			HASH_ADD_KEYPTR(hh, prog_symbols, s->name, strlen(s->name), s);
			
			free(l_start);
		}
	}
	
	fclose(f);
}

void load_lst(char *path) {
	FILE *f = fopen(path, "r");
	
	assert(f);
	
	bool cblock_found = false;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while((linelen = getline(&line, &linecap, f)) > 0) {
		char *loc = strstr(line, " cblock ");
		
		if(loc) {
			cblock_found = true;
			continue;
		}
		
		if(!cblock_found) {
			continue;
		}
		
		loc = strstr(line, " endc ");
		
		if(loc) {
			break;
		}
		
		if(linelen < 3) {
			continue;
		}
		
		if(line[2] != '0') {
			continue;
		}
		
		char *l_start = strdup(line);
		char *l = l_start;
		
		char *a = find_word(&l);
		if(!a) {
			free(l_start);
			break;
		}
		
		char *lineno = find_word(&l);
		if(!lineno) {
			free(l_start);
			break;
		}
		
		char *name = find_word(&l);
		if(!name) {
			free(l_start);
			break;
		}
		
		uint8_t address = strtol(a, NULL, 16);
		
		struct symbol *s = malloc(sizeof(struct symbol));
		s->name = strdup(name);
		s->address = address;
		HASH_ADD_KEYPTR(hh, ram_symbols, s->name, strlen(s->name), s);
		
		free(l_start);
	}
	
	fclose(f);
}

void load_files(char *dir) {
	char *project_name = basename(dir);
	char *dist_prefix;
	asprintf(&dist_prefix, "%s/dist/default/production/%s.production", dir, project_name);
	
	char *hex_file;
	asprintf(&hex_file, "%s.hex", dist_prefix);
	char *map_file;
	asprintf(&map_file, "%s.map", dist_prefix);
	
	char *lst_file;
	char *project_name_no_ext = strdup(project_name);
	project_name_no_ext[strlen(project_name_no_ext) - 2] = '\0';
	asprintf(&lst_file, "%s/build/default/production/%s.lst", dir, project_name_no_ext);
	
	load_hex(hex_file);
	load_map(map_file);
	load_lst(lst_file);
	
	free(dist_prefix);
	free(hex_file);
	free(map_file);
	free(project_name_no_ext);
	free(lst_file);
}
