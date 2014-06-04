#ifndef SERMENULET_H
#define SERMENULET_H

#import <Cocoa/Cocoa.h>

#import <Foundation/Foundation.h>

@interface MacMenu : NSView

- (void)setupMenuItem;
- (void)setupView:(NSView *)view;

@property (nonatomic, assign, getter = isActive) BOOL active;
@property (nonatomic, strong) NSPopover *popover;
@property (nonatomic, strong) NSStatusItem *item;
@property (nonatomic, strong) NSViewController *controller;
@property (nonatomic, strong) NSView *view;

@end


#endif // SERMENULET_H
