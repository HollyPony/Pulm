#import "macmenu.h"

#include <QWidget>

//static void *kActiveChangedKVO = &kActiveChangedKVO;

@implementation MacMenu

- (id)initWithView:(NSView *)view
{
    self.controller = [[NSViewController alloc]init];

    CGFloat thickness = [[NSStatusBar systemStatusBar] thickness];
    self.item = [[NSStatusBar systemStatusBar] statusItemWithLength:thickness];

    id result = [self initWithFrame:(NSRect){.size={thickness, thickness}}];
    [self.item setView: self];
    [self.controller setView: view];

    return result;

}

- (void)drawRect:(NSRect)rect
{
#if WITHOUT_IMAGE
    rect = CGRectInset(rect, 2, 2);
    if ([self isActive]) {
        [[NSColor selectedMenuItemColor] set]; /* blueish */
    } else {
        [[NSColor textColor] set]; /* blackish */
    }
    NSRectFill(rect);
#else
    NSImage *menuletIcon;
    [[NSColor clearColor] set];
    if ([self isActive]) {
        menuletIcon = [NSImage imageNamed:@":/icons/Moon_Full.png"];
    } else {
        menuletIcon = [NSImage imageNamed:@":/icons/Moon_New.png"];
    }
    [menuletIcon drawInRect:NSInsetRect(rect, 2, 2) fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1.0];
#endif
}

- (void)_setupPopover {
    if (!self.popover) {
        self.popover = [[NSPopover alloc] init];
        self.popover.contentViewController = self.controller;
        self.popover.contentSize =  [self.popover.contentViewController.view fittingSize];
    }
}

- (void)mouseDown:(NSEvent *)event {
    if (self.active) {
        [self _setupPopover];
        [self.popover showRelativeToRect:[self frame]
                                  ofView:self
                           preferredEdge:NSMinYEdge];

        self.active = ! self.active;
        NSLog(@"Open");
    } else {
        [self.popover performClose:self];
        self.active = ! self.active;
    }
}

@end
