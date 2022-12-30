/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#ifndef XINTUITION_H
#define XINTUITION_H

#include <X11/Xlib.h>

#define InitializeXIntuition()   (void)(0)
void StartupXIntuition(void);
void ShutdownXIntuition(void);
#define DestroyXIntuition()      (void)(0)

Window OpenBorderWindow(int x, int y, int width, int height, const char *title);
Display *GetIntuitionDisplay();
void SendXEventToIntuition(XEvent *event);
char IsSBarWindow(const char *className, const char *instanceName);

void SetBorderWindowTitle(Window w, const char *title);
void SetScreenTitle(const char *title);

#endif
