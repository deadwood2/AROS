/*
    Copyright (C) 2023, The AROS Development Team. All rights reserved.
*/

#ifndef CGFX_GFX_PRIVATE
#define CGFX_GFX_PRIVATE

static inline LONG __inline_Graphics_WritePixels8(struct RastPort * __arg1, UBYTE * __arg2, ULONG __arg3, WORD __arg4, WORD __arg5, WORD __arg6, WORD __arg7, APTR __arg8, BOOL __arg9, APTR __GfxBase)
{
    AROS_LIBREQ(GfxBase, 40)
    return AROS_LC9(LONG, WritePixels8,
        AROS_LCA(struct RastPort *,(__arg1),A0),
        AROS_LCA(UBYTE *,(__arg2),A1),
        AROS_LCA(ULONG,(__arg3),D0),
        AROS_LCA(WORD,(__arg4),D1),
        AROS_LCA(WORD,(__arg5),D2),
        AROS_LCA(WORD,(__arg6),D3),
        AROS_LCA(WORD,(__arg7),D4),
        AROS_LCA(APTR,(__arg8),A2),
        AROS_LCA(BOOL,(__arg9),D5),
        struct GfxBase *, (__GfxBase), 197, Graphics    );
}

#define WritePixels8(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9) \
    __inline_Graphics_WritePixels8((arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), (arg9), __aros_getbase_GfxBase())

static inline LONG __inline_Graphics_FillRectPenDrMd(struct RastPort * __arg1, WORD __arg2, WORD __arg3, WORD __arg4, WORD __arg5, ULONG __arg6, IPTR __arg7, BOOL __arg8, APTR __GfxBase)
{
    AROS_LIBREQ(GfxBase, 40)
    return AROS_LC8(LONG, FillRectPenDrMd,
        AROS_LCA(struct RastPort *,(__arg1),A0),
        AROS_LCA(WORD,(__arg2),D0),
        AROS_LCA(WORD,(__arg3),D1),
        AROS_LCA(WORD,(__arg4),D2),
        AROS_LCA(WORD,(__arg5),D3),
        AROS_LCA(ULONG,(__arg6),D4),
        AROS_LCA(IPTR,(__arg7),D5),
        AROS_LCA(BOOL,(__arg8),D6),
        struct GfxBase *, (__GfxBase), 198, Graphics    );
}

#define FillRectPenDrMd(arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8) \
    __inline_Graphics_FillRectPenDrMd((arg1), (arg2), (arg3), (arg4), (arg5), (arg6), (arg7), (arg8), __aros_getbase_GfxBase())

static inline LONG __inline_Graphics_DoRenderFunc(struct RastPort * __arg1, Point * __arg2, struct Rectangle * __arg3, APTR __arg4, APTR __arg5, BOOL __arg6, APTR __GfxBase)
{
    AROS_LIBREQ(GfxBase, 40)
    return AROS_LC6(LONG, DoRenderFunc,
        AROS_LCA(struct RastPort *,(__arg1),A0),
        AROS_LCA(Point *,(__arg2),A1),
        AROS_LCA(struct Rectangle *,(__arg3),A2),
        AROS_LCA(APTR,(__arg4),A3),
        AROS_LCA(APTR,(__arg5),A4),
        AROS_LCA(BOOL,(__arg6),D0),
        struct GfxBase *, (__GfxBase), 199, Graphics    );
}

#define DoRenderFunc(arg1, arg2, arg3, arg4, arg5, arg6) \
    __inline_Graphics_DoRenderFunc((arg1), (arg2), (arg3), (arg4), (arg5), (arg6), __aros_getbase_GfxBase())

static inline LONG __inline_Graphics_DoPixelFunc(struct RastPort * __arg1, WORD __arg2, WORD __arg3, APTR __arg4, APTR __arg5, BOOL __arg6, APTR __GfxBase)
{
    AROS_LIBREQ(GfxBase, 40)
    return AROS_LC6(LONG, DoPixelFunc,
        AROS_LCA(struct RastPort *,(__arg1),A0),
        AROS_LCA(WORD,(__arg2),D0),
        AROS_LCA(WORD,(__arg3),D1),
        AROS_LCA(APTR,(__arg4),A1),
        AROS_LCA(APTR,(__arg5),A2),
        AROS_LCA(BOOL,(__arg6),D2),
        struct GfxBase *, (__GfxBase), 200, Graphics    );
}

#define DoPixelFunc(arg1, arg2, arg3, arg4, arg5, arg6) \
    __inline_Graphics_DoPixelFunc((arg1), (arg2), (arg3), (arg4), (arg5), (arg6), __aros_getbase_GfxBase())

static inline void __inline_Graphics_UpdateBitMap(struct BitMap * __arg1, UWORD __arg2, UWORD __arg3, UWORD __arg4, UWORD __arg5, APTR __GfxBase)
{
    AROS_LIBREQ(GfxBase, 40)
    AROS_LC5NR(void, UpdateBitMap,
        AROS_LCA(struct BitMap *,(__arg1),A0),
        AROS_LCA(UWORD,(__arg2),D0),
        AROS_LCA(UWORD,(__arg3),D1),
        AROS_LCA(UWORD,(__arg4),D2),
        AROS_LCA(UWORD,(__arg5),D3),
        struct GfxBase *, (__GfxBase), 201, Graphics    );
}

#define UpdateBitMap(arg1, arg2, arg3, arg4, arg5) \
    __inline_Graphics_UpdateBitMap((arg1), (arg2), (arg3), (arg4), (arg5), __aros_getbase_GfxBase())

#endif
