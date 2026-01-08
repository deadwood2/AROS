/*
    Copyright (C) 2026, The AROS Development Team. All rights reserved.
*/

#ifndef _INTUITION_GADGETS
#define _INTUITION_GADGETS

#define GWD_KEY 0x3af401b3

struct GadgetWrapperData
{
    ULONG           gwd_Key;
    struct GadgetV0 *gwd_Wrapped;
};

void Intuition_Gadgets_init(struct IntuitionBaseV0 *abiv0IntuitionBase, APTR32 *intuitionjmp);
void Intuition_Gadgets_init_GadgetWrapper_class();

UWORD abiv0_AddGList(struct WindowV0 *window, struct GadgetV0 *gadget, ULONG position, LONG numGad, APTR /*struct RequesterV0 **/requester,
        struct LibraryV0 *IntuitionBaseV0);

#endif
