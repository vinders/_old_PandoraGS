/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : in-game language/translation utility
*******************************************************************************/
#pragma once

#include <string>
#include "i_lang.h"

/// @namespace lang
/// Language utilities
namespace lang
{
    /// @class MenuLang
    /// @brief Translation utility for in-game plugin menu
    class MenuLang : public ILang
    {
    public:
        //std::wstring ...

    public:
        /// @brief Create uninitialized language resource (for in-game menu)
        MenuLang() {}
        /// @brief Create language resource (for in-game menu)
        /// @param code Language code
        /// @param filePath Language file path
        MenuLang(langcode_t code, std::wstring& filePath)
        {
            setLanguage(code, filePath);
        }

        /// @brief Set english values
        virtual void setLanguageEnglish();
        /// @brief Set spanish values
        virtual void setLanguageSpanish();
        /// @brief Set french values
        virtual void setLanguageFrench();
        /// @brief Set german values
        virtual void setLanguageGerman();

        /// @brief Read values from file (english if not found)
        /// @param filePath Language file path
        virtual void setLanguageFromFile(std::wstring& filePath);
    };
}
