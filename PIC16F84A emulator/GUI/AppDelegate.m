//
//  AppDelegate.m
//  PIC16F84A Emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#import "AppDelegate.h"

#include "load.h"
#include "symbols.h"
#include "state.h"
#include "runtime.h"

#define BYTETOBINARYPATTERN "%d%d%d%d%d%d%d%d"
#define BYTETOBINARY(byte)  \
(byte & 0x80 ? 1 : 0), \
(byte & 0x40 ? 1 : 0), \
(byte & 0x20 ? 1 : 0), \
(byte & 0x10 ? 1 : 0), \
(byte & 0x08 ? 1 : 0), \
(byte & 0x04 ? 1 : 0), \
(byte & 0x02 ? 1 : 0), \
(byte & 0x01 ? 1 : 0)

NSNumber *NSNumberFromString(NSString *str) {
	str = [str stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceCharacterSet]];
	
	int base = 0;
	
	if([str hasPrefix:@"0b"]) {
		str = [str substringFromIndex:2];
		base = 2;
	}
	return @(strtoll([str UTF8String], NULL, base));
}

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	self.labels = [[NSMutableArray alloc] init];
	self.labelAddresses = [[NSMutableArray alloc] init];
	self.variables = [[NSMutableArray alloc] init];
	self.variableAddresses = [[NSMutableArray alloc] init];
	self.variableInputValues = [[NSMutableArray alloc] init];
	self.variableOutputValues = [[NSMutableArray alloc] init];
	
	self.labelTable.dataSource = self;
	self.variableInputTable.dataSource = self;
	self.variableOutputTable.dataSource = self;
	
	self.WInput.delegate = self;
	self.WOutput.delegate = self;
	
	[self.window orderOut:self];
}

- (IBAction)open:(id)sender {
	NSOpenPanel *openPanel = [NSOpenPanel openPanel];
	openPanel.canChooseDirectories = YES;
	openPanel.canChooseFiles = NO;
	
	[openPanel beginSheetModalForWindow:nil completionHandler:^(NSModalResponse response) {
		if(response != NSOKButton) {
			return;
		}
		[openPanel orderOut:self];
		[self loadFile:[openPanel URL]];
	}];
}

- (void)loadFile:(NSURL *)url {
	NSFileManager *fileManager = [NSFileManager defaultManager];
	
	NSString *projectName = [[url pathComponents] lastObject];
	
	NSString *distPrefix = [NSString stringWithFormat:@"%@/dist/default/production/%@.production", [url path], projectName];
	
	BOOL shouldLoad = YES;
	
	for(NSString *extension in @[@"map", @"hex"]) {
		NSString *filePath = [NSString stringWithFormat:@"%@.%@", distPrefix, extension];
		if(![fileManager fileExistsAtPath:filePath]) {
			NSAlert *alert = [NSAlert alertWithMessageText:@"Error loading project" defaultButton:@"OK" alternateButton:nil otherButton:nil informativeTextWithFormat:@"The file '%@' could not be found. Please make sure you have built your project with MPLABX!", filePath];
			[alert runModal];
			
			shouldLoad = NO;
			break;
		}
	}
	
	if(!shouldLoad) {
		return;
	}
	
	load_files([[url path] UTF8String]);
	
	[self.labels removeAllObjects];
	[self.labelAddresses removeAllObjects];
	[self.variables removeAllObjects];
	[self.variableAddresses removeAllObjects];
	[self.variableInputValues removeAllObjects];
	[self.variableOutputValues removeAllObjects];
	
	struct symbol *s, *tmp;
	
	HASH_ITER(hh_name, prog_symbols_by_name, s, tmp) {
		NSString *name = [NSString stringWithUTF8String:s->name];
		[self.labels addObject:name];
		[self.labelAddresses addObject:@(s->address)];
	}
	
	HASH_ITER(hh_name, ram_symbols_by_name, s, tmp) {
		NSString *name = [NSString stringWithUTF8String:s->name];
		[self.variables addObject:name];
		[self.variableAddresses addObject:@(s->address)];
		[self.variableInputValues addObject:[NSNull null]];
		[self.variableOutputValues addObject:[NSNull null]];
	}
	
	[self.labelTable reloadData];
	[self.variableInputTable reloadData];
	[self.variableOutputTable reloadData];
	
	[self.window makeKeyAndOrderFront:self];
}

- (IBAction)run:(id)sender {
	[self.runButton setEnabled:NO];
	
	initialize();
	reset();
	
	NSInteger selectedRow = self.labelTable.selectedRow;
	if(selectedRow == -1) {
		NSAlert *alert = [NSAlert alertWithMessageText:@"Error running" defaultButton:@"OK" alternateButton:nil otherButton:nil informativeTextWithFormat:@"Please select a label on the left before running!"];
		[alert runModal];
		return;
	}
	
	PC = [self.labelAddresses[selectedRow] unsignedIntValue];
	W = [NSNumberFromString(self.WInput.stringValue) unsignedIntValue];
	for(int i = 0; i < [self.variables count]; i++) {
		id value = self.variableInputValues[i];
		if([value isKindOfClass:[NSNumber class]]) {
			ram[[self.variableAddresses[i] unsignedIntValue]] = [value unsignedIntValue];
		}
	}
	
	run();
	
	self.WOutput.stringValue = [NSString stringWithFormat:@"0x%X", W];
	for(int i = 0; i < [self.variables count]; i++) {
		self.variableOutputValues[i] = @(ram[[self.variableAddresses[i] unsignedIntValue]]);
	}
	
	[self.variableOutputTable reloadData];
	
	[self.runButton setEnabled:YES];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
	if(aTableView == self.labelTable) {
		return [self.labels count];
	} else {
		return [self.variables count];
	}
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
	NSString *headerTitle =((NSTableHeaderCell *)aTableColumn.headerCell).title;
	NSArray *array;
	
	if(aTableView == self.labelTable) {
		if([headerTitle isEqualToString:@"Address"]) {
			array = self.labelAddresses;
		} else {
			array = self.labels;
		}
	} else {
		if([headerTitle isEqualToString:@"Address"]) {
			array = self.variableAddresses;
		} else if([headerTitle isEqualToString:@"Variable"]) {
			array = self.variables;
		} else if(aTableView == self.variableInputTable) {
			array = self.variableInputValues;
		} else {
			array = self.variableOutputValues;
		}
	}
	
	id element = array[rowIndex];
	
	if([element isKindOfClass:[NSString class]]) {
		return element;
	} else if([element isKindOfClass:[NSNull class]]) {
		return @"?";
	} else if([element isKindOfClass:[NSNumber class]]) {
		if([headerTitle isEqualToString:@"Binary"]) {
			return [NSString stringWithFormat:@"0b" BYTETOBINARYPATTERN, BYTETOBINARY([element unsignedIntValue])];
		}
		
		NSString *format;
		if([headerTitle isEqualToString:@"Decimal"]) {
			format = @"%d";
		} else {
			format = @"0x%X";
		}
		return [NSString stringWithFormat:format, [element unsignedIntValue]];
	} else {
		return nil;
	}
}

- (void)tableView:(NSTableView *)aTableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)aTableColumn row:(NSInteger)rowIndex {
	id value;
	
	if([anObject isEqualToString:@""]) {
		value = [NSNull null];
	} else {
		value = NSNumberFromString(anObject);
		if([value unsignedIntValue] > 255) {
			value = @(255);
		}
	}
	
	self.variableInputValues[rowIndex] = value;
}

- (BOOL)control:(NSControl *)control textShouldBeginEditing:(NSText *)fieldEditorn{
	return control == self.WInput;
}

- (BOOL)control:(NSControl *)control textShouldEndEditing:(NSText *)fieldEditor {
	if(control == self.WInput) {
		NSNumber *number = NSNumberFromString(self.WInput.stringValue);
		unsigned int value = [number unsignedIntValue];
		if(value > 255) {
			value = 255;
		}
		
		self.WInput.stringValue = [NSString stringWithFormat:@"0x%X", value];
	}
	
	return YES;
}

@end
