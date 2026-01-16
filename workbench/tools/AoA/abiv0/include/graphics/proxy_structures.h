/*
    Copyright (C) 2025-2026, The AROS Development Team. All rights reserved.
*/

#ifndef ABIV0_GRAPHICS_PROXY_STRUCTURES_H
#define ABIV0_GRAPHICS_PROXY_STRUCTURES_H

struct ColorMapProxy
{
    struct ColorMapV0   base;
    struct ColorMap     *native;
};

struct LayerProxy
{
    struct LayerV0  base;
    struct Layer    *native;
};

struct RegionProxy
{
    struct RegionV0 base;
    struct Region   *native;

    struct RegionRectangleV0    *rrArray;
    ULONG                       rrCount;
};

#define BITMAPPROXYKEY 0xE5A78151
struct BitMapProxy
{
    struct BitMapV0 base;
    ULONG           key;
    struct BitMap   *native;
};

struct TextFontProxy
{
    struct ColorTextFontV0  base; /* Allocated bigger (CTF) space just in case by default */
    struct TextFont         *native;
};

#endif
