/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   poly_prim.h
Description : polygon primitive management
*******************************************************************************/
#ifndef _POLY_PRIM_H
#define _POLY_PRIM_H
#include "drawing_prim.h"

class PolyPrim : public DrawingPrim
{
private:

public:
    PolyPrim();
    ~PolyPrim();
};

#endif
