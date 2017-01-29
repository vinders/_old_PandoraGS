/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : in-game language/translation - common interface
*******************************************************************************/
#pragma once

#include <cstdint>
#include <string>

#define LANG_DEFAULT lang::langcode_t::english
#define LANG_LAST_INTERNAL lang::langcode_t::german
#define LANG_NAMES_LIST {L" English",L" Español",L" Français",L" Deutsch",L" External file..."}

/// @namespace lang
/// Language utilities
namespace lang
{
    /// @enum langcode_t
    /// @brief Language codes
    enum class langcode_t : uint32_t // languages
    {
        english = 0,
        spanish = 1,
        french = 2,
        german = 3,
        customFile = 1000 // external file
    };


    /// @class MenuLang
    /// @brief Translation utility - interface
    class ILang
    {
    public:
        /// @brief Set game language values (necessary)
        /// @param code Language code
        /// @param filePath Language file path
        void setLanguage(langcode_t code, std::wstring& filePath)
        {
            switch (code)
            {
                case (langcode_t::english) : setLanguageEnglish(); break;
                case (langcode_t::spanish) : setLanguageSpanish(); break;
                case (langcode_t::french) : setLanguageFrench();  break;
                case (langcode_t::german) : setLanguageGerman();  break;
                case (langcode_t::customFile) : setLanguageFromFile(filePath); break;
                default: setLanguageEnglish(); break;
            }
        }

        /// @brief Set english values
        virtual void setLanguageEnglish() = 0;
        /// @brief Set spanish values
        virtual void setLanguageSpanish() = 0;
        /// @brief Set french values
        virtual void setLanguageFrench() = 0;
        /// @brief Set german values
        virtual void setLanguageGerman() = 0;

        /// @brief Read values from file
        /// @param filePath Language file path
        virtual void setLanguageFromFile(std::wstring& filePath) = 0;
    };
}
