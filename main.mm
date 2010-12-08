//
//  main.m
//  Trap
//
//  Created by Max Tkachenko on 10/5/10.
//  Copyright Maxim Tkachenko 2010. All rights reserved.
//

#import <UIKit/UIKit.h>

int main(int argc, char *argv[]) {
    NSLog(@"Trap started...");
    NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];
    int retVal = UIApplicationMain(argc, argv, nil, nil);
    [pool release];
    return retVal;
}
