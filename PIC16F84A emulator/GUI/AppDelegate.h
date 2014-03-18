//
//  AppDelegate.h
//  PIC16F84A Emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, NSTableViewDataSource, NSTextFieldDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSTableView *labelTable;
@property (weak) IBOutlet NSTableView *variableInputTable;
@property (weak) IBOutlet NSTableView *variableOutputTable;
@property (weak) IBOutlet NSTextField *WInput;
@property (weak) IBOutlet NSTextField *WOutput;
@property (weak) IBOutlet NSButton *runButton;

- (IBAction)open:(id)sender;
- (IBAction)run:(id)sender;

@property NSMutableArray *labels;
@property NSMutableArray *labelAddresses;
@property NSMutableArray *variables;
@property NSMutableArray *variableAddresses;
@property NSMutableArray *variableInputValues;
@property NSMutableArray *variableOutputValues;

@end
