/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   sprite_prim.h
Description : sprite primitive management
*******************************************************************************/
#ifndef _SPRITE_PRIM_H
#define _SPRITE_PRIM_H
#include "drawing_prim.h"

class SpritePrim : public DrawingPrim
{
private:

public:
    SpritePrim();
    ~SpritePrim();
};

#endif
