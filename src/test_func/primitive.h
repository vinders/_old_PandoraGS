/*******************************************************************************
PANDORAGS project - PS1 GPU driver test app
------------------------------------------------------------------------
File name :   primitive.cpp
Description : test app - flow/unit tests -- primitive list/creation functions
*******************************************************************************/
#pragma once

///<summary>Show full list of available primitives</summary>
void listPrimitives();

///<summary>Create a primitive</summary>
///<param name="id">Primitive ID (type)</param>
///<param name="pOut">Output reference</param>
///<returns>Primitive length</returns>
int createPrimitive(int id, unsigned char** pOut);

///<summary>Check if primitive can be flipped (rectangle)</summary>
///<param name="id">Primitive ID (type)</param>
bool isPrimitiveFlippable(int id);
