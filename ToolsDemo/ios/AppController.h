//
//  ToolsDemoAppController.h
//  ToolsDemo
//
//  Created by Nonlinear Ideas Inc. on 9/17/13.
//  Copyright Nonlinear Ideas Inc. 2013. All rights reserved.
//

@class RootViewController;

@interface AppController : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate> {
    UIWindow *window;
    RootViewController    *viewController;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) RootViewController *viewController;

@end

