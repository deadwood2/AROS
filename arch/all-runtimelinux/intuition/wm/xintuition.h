/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#ifndef XINTUITION_H
#define XINTUITION_H

#include <X11/Xlib.h>

Window OpenBorderWindow(int x, int y, int width, int height, const char *title);
Display *GetIntuitionDisplay();

#define InitializeXIntuition()   (void)(0)
void StartupXIntuition(void);
#define ShutdownXIntuition       (void)(0)
#define DestroyXIntuition()      (void)(0)

#endif
