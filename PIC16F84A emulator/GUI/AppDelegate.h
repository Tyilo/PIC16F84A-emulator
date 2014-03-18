//
//  AppDelegate.h
//  PIC16F84A Emulator
//
//  Created by Asger Hautop Drewsen on 18/03/2014.
//  Copyright (c) 2014 Tyilo. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate, NSTableViewDataSource>

@property (assign) IBOutlet NSWindow *window;
@property (weak) IBOutlet NSTableView *functionList;

- (IBAction)open:(id)sender;

@property NSArray *functions;

@end
