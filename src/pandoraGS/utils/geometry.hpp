/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   geometry.hpp
Description : geometry toolset
*******************************************************************************/
#ifndef _GEOMETRY_HPP
#define _GEOMETRY_HPP

// wide geometry point (dword)
typedef struct LPOINTTAG
{
    long x;
    long y;
    LPOINTTAG() : x(0L), y(0L) {}
    LPOINTTAG(long lx, long ly) : x(lx), y(ly) {}
} wpoint_t;
typedef wpoint_t wdimension_t;
// geometry point (word)
typedef struct HPOINTTAG
{
    short x;
    short y;
    HPOINTTAG() : x(0), y(0) {}
    HPOINTTAG(short sx, short sy) : x(sx), y(sy) {}
} point_t;
typedef point_t dimension_t;
// geometry point (float)
typedef struct FPOINTTAG
{
    float x;
    float y;
    FPOINTTAG() : x(0.0f), y(0.0f) {}
    FPOINTTAG(float fx, float fy) : x(fx), y(fy) {}
} fpoint_t;

// geometry line (dword)
typedef struct LLINETAG
{
    long x0;
    long y0;
    long x1;
    long y1;
    LLINETAG() : x0(0L), y0(0L), x1(0L), y1(0L) {}
    LLINETAG(long lx0, long ly0, long lx1, long ly1) : x0(lx0), y0(ly0), x1(lx1), y1(ly1) {}
} wline_t;
// geometry line (word)
typedef struct HLINETAG
{
    short x0;
    short y0;
    short x1;
    short y1;
    HLINETAG() : x0(0), y0(0), x1(0), y1(0) {}
    HLINETAG(short sx0, short sy0, short sx1, short sy1) : x0(sx0), y0(sy0), x1(sx1), y1(sy1) {}
} line_t;
// geometry line (float)
typedef struct FLINETAG
{
    float x0;
    float y0;
    float x1;
    float y1;
    FLINETAG() : x0(0.0f), y0(0.0f), x1(0.0f), y1(0.0f) {}
    FLINETAG(float fx0, float fy0, float fx1, float fy1) : x0(fx0), y0(fy0), x1(fx1), y1(fy1) {}
} fline_t;

// geometry triangle (dword)
typedef struct LTRITAG
{
    long x0;
    long y0;
    long x1;
    long y1;
    long x2;
    long y2;
    LTRITAG() : x0(0L), y0(0L), x1(0L), y1(0L), x2(0L), y2(0L) {}
    LTRITAG(long lx0, long ly0, long lx1, long ly1, long lx2, long ly2) : x0(lx0), y0(ly0), x1(lx1), y1(ly1), x2(lx2), y2(ly2) {}
} wtriangle_t;
// geometry triangle (word)
typedef struct HTRITAG
{
    short x0;
    short y0;
    short x1;
    short y1;
    short x2;
    short y2;
    HTRITAG() : x0(0), y0(0), x1(0), y1(0), x2(0), y2(0) {}
    HTRITAG(short sx0, short sy0, short sx1, short sy1, short sx2, short sy2) : x0(sx0), y0(sy0), x1(sx1), y1(sy1), x2(sx2), y2(sy2) {}
} triangle_t;
// geometry triangle (float)
typedef struct FTRITAG
{
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    FTRITAG() : x0(0.0f), y0(0.0f), x1(0.0f), y1(0.0f), x2(0.0f), y2(0.0f) {}
    FTRITAG(float fx0, float fy0, float fx1, float fy1, float fx2, float fy2) : x0(fx0), y0(fy0), x1(fx1), y1(fy1), x2(fx2), y2(fy2) {}
} ftriangle_t;

// geometry quad (dword)
typedef struct LQUADTAG
{
    long x0;
    long y0;
    long x1;
    long y1;
    long x2;
    long y2;
    long x3;
    long y3;
    LQUADTAG() : x0(0L), y0(0L), x1(0L), y1(0L), x2(0L), y2(0L), x3(0L), y3(0L) {}
    LQUADTAG(long lx0, long ly0, long lx1, long ly1, long lx2, long ly2, long lx3, long ly3) : x0(lx0), y0(ly0), x1(lx1), y1(ly1), x2(lx2), y2(ly2), x3(lx3), y3(ly3) {}
} wquad_t;
// geometry quad (word)
typedef struct HQUADTAG
{
    short x0;
    short y0;
    short x1;
    short y1;
    short x2;
    short y2;
    short x3;
    short y3;
    HQUADTAG() : x0(0), y0(0), x1(0), y1(0), x2(0), y2(0), x3(0), y3(0) {}
    HQUADTAG(short sx0, short sy0, short sx1, short sy1, short sx2, short sy2, short sx3, short sy3) : x0(sx0), y0(sy0), x1(sx1), y1(sy1), x2(sx2), y2(sy2), x3(sx3), y3(sy3) {}
} quad_t;
// geometry quad (float)
typedef struct FQUADTAG
{
    float x0;
    float y0;
    float x1;
    float y1;
    float x2;
    float y2;
    float x3;
    float y3;
    FQUADTAG() : x0(0.0f), y0(0.0f), x1(0.0f), y1(0.0f), x2(0.0f), y2(0.0f), x3(0.0f), y3(0.0f) {}
    FQUADTAG(float fx0, float fy0, float fx1, float fy1, float fx2, float fy2, float fx3, float fy3) : x0(fx0), y0(fy0), x1(fx1), y1(fy1), x2(fx2), y2(fy2), x3(fx3), y3(fy3) {}
} fquad_t;

// wide geometry tile (dword)
typedef struct LTILETAG
{
    long x;
    long y;
    long width;
    long height;
    LTILETAG() : x(0L), y(0L), width(0L), height(0L) {}
    LTILETAG(long lx, long ly, long lw, long lh) : x(lx), y(ly), width(lw), height(lh) {}
} wtile_t;
// geometry tile (word)
typedef struct HTILETAG
{
    short x;
    short y;
    short width;
    short height;
    HTILETAG() : x(0), y(0), width(0), height(0) {}
    HTILETAG(short sx, short sy, short sw, short sh) : x(sx), y(sy), width(sw), height(sh) {}
} tile_t;
// geometry tile (float)
typedef struct FTILETAG
{
    float x;
    float y;
    float width;
    float height;
    FTILETAG() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
    FTILETAG(float fx, float fy, float fw, float fh) : x(fx), y(fy), width(fw), height(fh) {}
} ftile_t;

// wide geometry rectangle (dword)
typedef struct LRECTTAG
{
    long x0; // X start
    long x1; // X end
    long y0; // Y start
    long y1; // Y end
    LRECTTAG() : x0(0L), x1(0L), y0(0L), y1(0L) {}
    LRECTTAG(long lx0, long lx1, long ly0, long ly1) : x0(lx0), x1(lx1), y0(ly0), y1(ly1) {}
} wrect_t;
// geometry rectangle (word)
typedef struct HRECTTAG
{
    short x0; // X start
    short x1; // X end
    short y0; // Y start
    short y1; // Y end
    HRECTTAG() : x0(0), x1(0), y0(0), y1(0) {}
    HRECTTAG(short sx0, short sx1, short sy0, short sy1) : x0(sx0), x1(sx1), y0(sy0), y1(sy1) {}
} rect_t;

#endif
