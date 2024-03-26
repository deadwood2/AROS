#ifndef ABIV0_GRAPHICS_PROXY_STRUCTURES_H
#define ABIV0_GRAPHICS_PROXY_STRUCTURES_H

struct ColorMapProxy
{
    struct ColorMap *native;
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

#endif
