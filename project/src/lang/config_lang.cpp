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
#include <stdexcept>
#include "config_lang.h"
using namespace lang;


/// @brief Set english values
void ConfigLang::setLanguageEnglish() noexcept
{

}

/// @brief Set spanish values
void ConfigLang::setLanguageSpanish() noexcept
{

}

/// @brief Set french values
void ConfigLang::setLanguageFrench() noexcept
{

}

/// @brief Set german values
void ConfigLang::setLanguageGerman() noexcept
{

}

/// @brief Read values from file (error if not found)
/// @param[in] filePath  Language file path
/// @throws invalid_argument  File not found or not accessible
void ConfigLang::setLanguageFromFile(const std::wstring& filePath)
{
    setLanguageEnglish(); // default values

    // open language file
    std::ifstream in;
    in.open(filePath, std::ifstream::in); // overwrite
    if (!in.is_open())
        throw std::invalid_argument("File not found or not accessible.");

    //...read file

    in.close();
}