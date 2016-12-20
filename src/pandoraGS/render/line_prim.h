/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   line_prim.h
Description : line primitive management
*******************************************************************************/
#ifndef _LINE_PRIM_H
#define _LINE_PRIM_H
#include "drawing_prim.h"

class LinePrim : public DrawingPrim
{
private:

public:
    LinePrim();
    ~LinePrim();
};

#endif
