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
#include <glob.h>

#include "state.h"
#include "utils.h"
#include "symbols.h"

#define CONFIG_ADDRESS 0x400e

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

void load_hex(const char *path) {
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
		if(address == CONFIG_ADDRESS) {
			continue;
		}
		
		uint64_t max_address = (address + data_len) / 2;
		if(max_address > LENGTH(prog_mem)) {
			printf("WARNING: Ignored hex field with address 0x%llx\n", address);
			continue;
		}
		
		for(int i = 0; i < data_len / 2; i++) {
			prog_mem[address / 2 + i] = __builtin_bswap16(atoi_n(&line[9 + i * 4], 4));
		}
	}
	
	fclose(f);
}

char *next_word(char **str) {
	char *token;
	
	while((token = strsep(str, " \n\t\r")) && token[0] == '\0') {
		
	}

	return token;
}

const char *find_word(const char *str, const char *word) {
	char *s = strdup(str);
	char *s_start = s;
	
	char *w;
	while((w = next_word(&s))) {
		if(strcasecmp(word, w) == 0) {
			free(s_start);
			return str + (w - s_start);
		}
	}
	
	free(s_start);
	
	return NULL;
}

void load_map(const char *path) {
	FILE *f = fopen(path, "r");
	
	assert(f);
	
	int header_lines_ago = -1;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while((linelen = getline(&line, &linecap, f)) > 0) {
		if(strstr(line, "Symbols - Sorted by Address")) {
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
			char *name = next_word(&l);
			
			if(!name) {
				free(l_start);
				break;
			}
			
			char *a = next_word(&l);
			
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

void load_lst(const char *path) {
	FILE *f = fopen(path, "r");
	
	assert(f);
	
	bool inside_cblock = false;
	
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while((linelen = getline(&line, &linecap, f)) > 0) {
		if(find_word(line, "cblock")) {
			inside_cblock = true;
			continue;
		}
		
		if(!inside_cblock && !find_word(line, "equ")) {
			continue;
		}
		
		if(find_word(line, "endc")) {
			inside_cblock = false;
			continue;
		}
		
		if(linelen < 3 || line[2] != '0') {
			continue;
		}
		
		char *l_start = strdup(line);
		char *l = l_start;
		
		char *a = next_word(&l);
		if(!a) {
			free(l_start);
			continue;
		}
		
		char *lineno = next_word(&l);
		if(!lineno) {
			free(l_start);
			continue;
		}
		
		char *name = next_word(&l);
		
		if(!name) {
			free(l_start);
			continue;
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

void load_files(const char *dir) {
	char *project_name = basename((char *)dir);
	char *dist_prefix;
	asprintf(&dist_prefix, "%s/dist/default/production/%s.production", dir, project_name);
	
	char *hex_file;
	asprintf(&hex_file, "%s.hex", dist_prefix);
	char *map_file;
	asprintf(&map_file, "%s.map", dist_prefix);
	
	load_hex(hex_file);
	load_map(map_file);
	
	char *lst_pattern;
	asprintf(&lst_pattern, "%s/build/default/production/*.lst", dir);
	glob_t pglob;
	glob(lst_pattern, 0, NULL, &pglob);
	
	for(int i = 0; i < pglob.gl_matchc; i++) {
		load_lst(pglob.gl_pathv[i]);
	}
	
	free(dist_prefix);
	free(hex_file);
	free(map_file);
	free(lst_pattern);
	globfree(&pglob);
}
