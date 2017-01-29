/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : config dialog language/translation utility
*******************************************************************************/
#include "../globals.h"
#include <iostream>
#include <fstream>
#include <string>
#include "../events/utils/std_exception.hpp"
#include "config_lang.h"
using namespace lang;


/// @brief Set english values
void ConfigLang::setLanguageEnglish()
{

}

/// @brief Set spanish values
void ConfigLang::setLanguageSpanish()
{

}

/// @brief Set french values
void ConfigLang::setLanguageFrench()
{

}

/// @brief Set german values
void ConfigLang::setLanguageGerman()
{

}

/// @brief Read values from file (error if not found)
/// @param filePath Language file path
/// @throw File not found or not accessible
void ConfigLang::setLanguageFromFile(std::wstring& filePath)
{
    setLanguageEnglish(); // default values

    // open language file
    std::ifstream in;
    in.open(filePath, std::ifstream::in); // overwrite
    if (!in.is_open())
        throw events::utils::StdException("File not found or not accessible.");

    //...read file

    in.close();
}