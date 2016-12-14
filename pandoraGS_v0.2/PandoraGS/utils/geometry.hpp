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
    LPOINTTAG() : x(0), y(0) {}
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

// wide geometry rectangle (dword)
typedef struct LRECTTAG
{
    long x0; // X start
    long x1; // X end
    long y0; // Y start
    long y1; // Y end
    LRECTTAG() : x0(0), x1(0), y0(0), y1(0) {}
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

// wide geometry quad (dword)
typedef struct LQUADTAG
{
    long x;
    long y;
    long width;
    long height;
    LQUADTAG() : x(0), y(0), width(0), height(0) {}
    LQUADTAG(long lx, long ly, long lw, long lh) : x(lx), y(ly), width(lw), height(lh) {}
} wquad_t;

// geometry quad (word)
typedef struct HQUADTAG
{
    short x;
    short y;
    short width;
    short height;
    HQUADTAG() : x(0), y(0), width(0), height(0) {}
    HQUADTAG(short sx, short sy, short sw, short sh) : x(sx), y(sy), width(sw), height(sh) {}
} quad_t;

#endif
