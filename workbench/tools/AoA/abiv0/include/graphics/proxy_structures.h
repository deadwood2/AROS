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
};

struct BitMapProxy
{
    struct BitMapV0 base;
    struct BitMap   *native;
};

struct TextFontProxy
{
    struct ColorTextFontV0  base; /* Allocated bigger (CTF) space just in case by default */
    struct TextFont         *native;
};

#endif
