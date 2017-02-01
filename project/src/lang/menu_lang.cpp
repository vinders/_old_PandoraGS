/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : in-game menu language/translation utility
*******************************************************************************/
#include "../globals.h"
#include <iostream>
#include <fstream>
#include <string>
#include "../events/utils/logger.h"
#include "menu_lang.h"
using namespace lang;


/// @brief Set english values
void MenuLang::setLanguageEnglish()
{

}

/// @brief Set spanish values
void MenuLang::setLanguageSpanish()
{

}

/// @brief Set french values
void MenuLang::setLanguageFrench()
{

}

/// @brief Set german values
void MenuLang::setLanguageGerman()
{

}

/// @brief Read values from file (english if not found)
/// @param filePath Language file path
void MenuLang::setLanguageFromFile(std::wstring& filePath)
{
    setLanguageEnglish(); // default values

    try
    {
        // open language file
        std::ifstream in;
        in.open(filePath, std::ifstream::in);
        if (!in.is_open())
        {
            events::utils::Logger::getInstance()->writeErrorEntry("MenuLang.setLanguageFromFile", "File not found or not accessible.");
            return;
        }

        //...read file

        in.close();
    }
    catch (const std::exception& exc) // in-game -> no exception
    {
        events::utils::Logger::getInstance()->writeErrorEntry("MenuLang.setLanguageFromFile", exc.what());
    }
}
