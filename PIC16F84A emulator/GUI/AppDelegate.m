//
//  AppDelegate.m
//  PIC16F84A Emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#import "AppDelegate.h"

#include "load.h"

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
	
	self.functions = @[];
	
	self.functionList.dataSource = self;
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
	load_files([[url path] UTF8String]);
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView {
	return [self.functions count];
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row {
	return self.functions[row];
}

@end
