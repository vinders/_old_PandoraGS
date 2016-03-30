//------------------------------------------------------------------------------
// PLAYSTATION EXE NAME SPECIFICATION (for PSE GPU plugins)
//------------------------------------------------------------------------------
// This specification allows the GPU plugin to know the name of the current
// game executable. It can be useful for debug and for config associations.
// All you need to make it work is to implement (and define as exported)
// a function that has the following prototype :
//      void CALLBACK GPUsetExeName(char* pName);

// The emulator calls GPUsetExeName (if defined) just before calling GPUopen
// -> that way, you can test in GPUopen which executable was launched.
// If the emulator does not support the function, it will simply not call it
// -> the global variable will still have a NULL value in that case.

// Make sure to test if the 'gpu_gameExeName' var has a value before using it
// -> that way, if an emulator doesn't support the function, it won't crash
//      if (gpu_gameExeName)
//      {
//          // use gpu_gameExeName here (will just be ignored if unsupported)
//      }

//------------------------------------------------------------------------------

// The rest of this file contains my own implementation of the specification.
// Feel free to use it the way you want.

// HOW TO USE THE SPEC :
// - add an entry for the GPUsetExeName function into the project.def file
// - add the following content into 'gpu.c' (or equivalent)

//------------------------------------------------------------------------------

// required headers
#include <stdlib.h> // only for C version, not needed with C++ new/delete
#include <string.h>

// global variable
char*    gpu_gameExeName;


//...

long CALLBACK GPUinit() // don't replace your original, just complete it
{
    gpu_gameExeName = NULL;
    //... (keep the other stuff that was already there)
}

//...

long CALLBACK GPUclose() // don't replace your original, just complete it
{
    //... (keep the other stuff that was already there)

    /* - C legacy version (if C compiler) - */
        if (gpu_gameExeName)
        {
            free(gpu_gameExeName);
        }
    /* - C++ modern version (if C++ compiler) -
        if (gpu_gameExeName)
            delete [] gpu_gameExeName;
    */
    gpu_gameExeName  = NULL;

    //... (keep the other stuff that was already there)
}

//...


// executable name setter
//-----------------------
// how it works :
// - checks if variable already defined -> free memory if it's the case
// - creates new memory allocation (one byte longer than pName length)
// - copies pName content into allocated memory
void CALLBACK GPUsetExeName(char* pName) // new function to create
{
    /* - C legacy version (if C compiler) - */
        if (gpu_gameExeName)
        {
            free(gpu_gameExeName);
        }
        gpu_gameExeName = (char*)malloc( (strlen(pName) + 1)*sizeof(char) );
    /* - C++ modern version (if C++ compiler) -
        if (gpu_gameExeName)
            delete [] gpu_gameExeName;
        gpu_gameExeName = new char[strlen(pName) + 1];
    */
    gpu_gameExeName[strlen(pName)] = '\0';
    strcpy(gpu_gameExeName, pName);
}
