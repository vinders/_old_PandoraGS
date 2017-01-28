/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : drawing attribute (area / transparency)
*******************************************************************************/
#include "primitive_facade.h"
#include "attribute.h"
using namespace command::primitive;


// -- attribute types - frame buffer settings -- ---------------------------

/// @brief Process draw mode / texture page change
/// @param pData Raw attribute data pointer
void attr_texpage_t::process(cmd_block_t* pData)
{
    attr_texpage_t* pAttr = (attr_texpage_t*)pData;

    //...
}

/// @brief Process texture window change
/// @param pData Raw attribute data pointer
void attr_texwin_t::process(cmd_block_t* pData)
{
    attr_texwin_t* pAttr = (attr_texwin_t*)pData;

    //...
}

/// @brief Process drawing area change
/// @param pData Raw attribute data pointer
void attr_drawarea_t::process(cmd_block_t* pData)
{
    attr_drawarea_t* pAttr = (attr_drawarea_t*)pData;

    //...
}

/// @brief Process drawing offset modification
/// @param pData Raw attribute data pointer
void attr_drawoffset_t::process(cmd_block_t* pData)
{
    attr_drawoffset_t* pAttr = (attr_drawoffset_t*)pData;

    //...
}

/// @brief Process semi-transparency bit change
/// @param pData Raw attribute data pointer
void attr_stpmask_t::process(cmd_block_t* pData)
{
    attr_stpmask_t* pAttr = (attr_stpmask_t*)pData;

    //...
}

/// @brief Process GPU interrupt request flag
/// @param pData Raw attribute data pointer
void attr_irqflag_t::process(cmd_block_t* pData)
{
    // vérif si IRQ1 activé
    // si oui, positionner flag d'interruption
    //...
}
