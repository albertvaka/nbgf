#include "macos_bridge.h"

#import <Foundation/Foundation.h>

#include "debug.h"

vec HelloWorld() {
    NSString *name = @"from Objective-C";
    NSString *greeting = [NSString stringWithFormat:@"Hello, %@!", name];
    NSLog(@"%@", greeting);
    return vec(1.0f, 1.0f);
}
