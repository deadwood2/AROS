/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef _INTUITION_SCREENS
#define _INTUITION_SCREENS

void Intuition_Screens_init(struct IntuitionBaseV0 *abiv0IntuitionBase, APTR32 *intuitionjmp);

void Intuition_Screens_init_first_screen(struct IntuitionBaseV0 *IntuitionBaseV0);

struct Screen *screenRemapV02N(struct ScreenV0 *v0screen);
struct ScreenV0 *screenRemapN2V0(struct Screen *nscreen);

#endif
