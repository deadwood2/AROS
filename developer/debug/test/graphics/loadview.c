/*
    Copyright (C) 2022, The AROS Development Team. All rights reserved.
*/

#include <proto/exec.h>
#include <proto/graphics.h>
#include <graphics/gfxbase.h>
#include <proto/dos.h>

int main()
{
    struct GfxBase *GfxBase;

    GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 0L);

    struct View *old = GfxBase->ActiView;

    LoadView(NULL);

    Delay(50);

    LoadView(old);

    CloseLibrary((struct Library *)GfxBase);

    return 0;
}
