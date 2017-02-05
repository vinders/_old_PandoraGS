/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : line primitive (line / poly-line)
*******************************************************************************/
#include "../../globals.h"
#include "primitive_facade.h"
#include "line_primitive.h"
using namespace command::primitive;


// -- primitive units - lines -- ---------------------------------------

/// @brief Process flat-shaded line
/// @param[in] pData  Raw primitive data pointer
void line_f2_t::process(command::cmd_block_t* pData)
{
    line_f2_t* pPrim = (line_f2_t*)pData;

    //...
}

/// @brief Process gouraud-shaded line
/// @param[in] pData  Raw primitive data pointer
void line_g2_t::process(command::cmd_block_t* pData)
{
    line_g2_t* pPrim = (line_g2_t*)pData;

    //...
}


// -- primitive units - poly-lines -- ----------------------------------

/// @brief Process flat-shaded poly-line
/// @param[in] pData  Raw primitive data pointer
void line_fp_t::process(command::cmd_block_t* pData)
{
    line_fp_t* pPrim = (line_fp_t*)pData;
    line_fp_iterator it(*pPrim);
    //...

    do
    {
        it.read();//...
    } while (it.next());
}

/// @brief Process gouraud-shaded poly-line
/// @param[in] pData  Raw primitive data pointer
void line_gp_t::process(command::cmd_block_t* pData)
{
    line_gp_t* pPrim = (line_gp_t*)pData;
    line_gp_iterator it(*pPrim);
    //...

    do
    {
        it.read();//...
    } while (it.next());
}
