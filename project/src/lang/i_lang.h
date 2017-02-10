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
using namespace std::literals::string_literals;

#define LANGCODE_DEFAULT lang::langcode_t::english

/// @namespace lang
/// Language utilities
namespace lang
{
    /// @enum langcode_t
    /// @brief Language codes
    enum class langcode_t : uint32_t
    {
        english = 0u,
        spanish = 1u,
        french = 2u,
        german = 3u,
        customFile = 0x0FFFFu ///< External file
    };
    #define LANGCODE_LAST_INTERNAL lang::langcode_t::german

    /// @brief Language names
    const std::wstring langcodeNames[] = { L" English"s, L" Español"s, L" Français"s, L" Deutsch"s };


    /// @class MenuLang
    /// @brief Translation utility - interface
    class ILang
    {
    public:
        /// @brief Set game language values (necessary)
        /// @param[in] code      Language code
        /// @param[in] filePath  Language file path
        void setLanguage(const langcode_t code, const std::wstring& filePath)
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
        virtual void setLanguageEnglish() noexcept = 0;
        /// @brief Set spanish values
        virtual void setLanguageSpanish() noexcept = 0;
        /// @brief Set french values
        virtual void setLanguageFrench() noexcept = 0;
        /// @brief Set german values
        virtual void setLanguageGerman() noexcept = 0;

        /// @brief Read values from file
        /// @param[in] filePath  Language file path
        virtual void setLanguageFromFile(const std::wstring& filePath) = 0;
    };
}
