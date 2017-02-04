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
    /// @see ILang
    class MenuLang : public ILang
    {
    public:
        //std::wstring ...

    public:
        /// @brief Create uninitialized language resource (for in-game menu)
        MenuLang() {}
        /// @brief Create language resource (for in-game menu)
        /// @param[in] code      Language code
        /// @param[in] filePath  Reference to language file path
        MenuLang(const langcode_t code, const std::wstring& filePath)
        {
            setLanguage(code, filePath);
        }

        /// @brief Set english values
        virtual void setLanguageEnglish() noexcept override;
        /// @brief Set spanish values
        virtual void setLanguageSpanish() noexcept override;
        /// @brief Set french values
        virtual void setLanguageFrench() noexcept override;
        /// @brief Set german values
        virtual void setLanguageGerman() noexcept override;

        /// @brief Read values from file (english if not found)
        /// @param[in] filePath  Language file path
        virtual void setLanguageFromFile(const std::wstring& filePath) override;
    };
}
