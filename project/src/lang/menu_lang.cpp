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
using namespace std::literals::string_literals;
#include "../events/utils/logger.h"
#include "menu_lang.h"
using namespace lang;


/// @brief Set english values
void MenuLang::setLanguageEnglish() noexcept
{

}

/// @brief Set spanish values
void MenuLang::setLanguageSpanish() noexcept
{

}

/// @brief Set french values
void MenuLang::setLanguageFrench() noexcept
{

}

/// @brief Set german values
void MenuLang::setLanguageGerman() noexcept
{

}

/// @brief Read values from file (english if not found)
/// @param[in] filePath  Language file path
void MenuLang::setLanguageFromFile(const std::wstring& filePath)
{
    setLanguageEnglish(); // default values

    try
    {
        // open language file
        std::ifstream in;
        in.open(filePath, std::ifstream::in);
        if (!in.is_open())
        {
            events::utils::Logger::getInstance()->writeErrorEntry("MenuLang.setLanguageFromFile"s, "File not found or not accessible."s);
            return;
        }

        //...read file

        in.close();
    }
    catch (const std::exception& exc) // in-game -> no exception
    {
        events::utils::Logger::getInstance()->writeErrorEntry("MenuLang.setLanguageFromFile"s, exc.what());
    }
}
