/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : config dialog language/translation utility
*******************************************************************************/
#pragma once

#include <string>
#include "i_lang.h"

/// @namespace lang
/// Language utilities
namespace lang
{
    /// @class MenuLang
    /// @brief Translation utility for config dialog
    class ConfigLang : public ILang
    {
    public:
        //std::wstring ...

    public:
        /// @brief Create uninitialized language resource (for config dialog)
        ConfigLang() {}
        /// @brief Create language resource (for config dialog)
        /// @param code Language code
        /// @param filePath Language file path (optional)
        ConfigLang(langcode_t code, std::wstring filePath = L"pandoraGS.lang")
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

        /// @brief Read values from file (error if not found)
        /// @param filePath Language file path
        /// @throw File not found or not accessible
        virtual void setLanguageFromFile(std::wstring& filePath);
    };
}
