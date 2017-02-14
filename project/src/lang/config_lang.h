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
    /// @see ILang
    class ConfigLang : public ILang
    {
    public:
        /// @struct dialog_t
        /// @brief Dialog related global translations
        struct dialog_t
        {
            std::wstring confirm;
            std::wstring cancel;
            std::wstring generalSettings;
            std::wstring profileManagement;
            std::wstring profileSettings;
        };
        /// @struct profile_t
        /// @brief Profile related global translations
        struct profile_t
        {
            std::wstring profileList;
            std::wstring filteringTab;
            std::wstring screenTab;
            std::wstring compatibilityTab;
        };
        /// @struct general_content_t
        /// @brief General settings translations
        struct general_content_t
        {
            std::wstring groupDisplay;
            std::wstring groupTimer;
        };
        /// @struct manager_content_t
        /// @brief Profile manager translations
        struct manager_content_t
        {
            std::wstring groupList;
        };
        /// @struct profile_content_t
        /// @brief Profile settings translations
        struct profile_content_t
        {
            std::wstring groupSmoothing;
            std::wstring groupRendering;
            std::wstring groupHdr;
            std::wstring groupMisc;
            std::wstring groupColor;
            std::wstring groupCompatibility;
        };
        /// @struct profile_content_t
        /// @brief Profile settings translations
        struct screen_content_t
        {
            std::wstring internalRes;
        };
        /// @struct profile_content_t
        /// @brief Profile settings translations
        struct compatibility_content_t
        {
            std::wstring groupColor;
            std::wstring groupCompatibility;
        };


    public:
        dialog_t dialog;   ///< Dialog related global translation
        profile_t profile; ///< Profile related global translations
        general_content_t generalSettings; ///< General settings translations
        manager_content_t profileManager; ///< Profile manager translations
        profile_content_t filteringSettings; ///< Profile settings translations
        screen_content_t screenSettings; ///< Profile settings translations
        compatibility_content_t compatibilitySettings; ///< Profile settings translations


    public:
        /// @brief Create uninitialized language resource (for config dialog)
        ConfigLang() {}
        /// @brief Create language resource (for config dialog)
        /// @param[in] code      Language code
        /// @param[in] filePath  Language file path (optional)
        ConfigLang(const langcode_t code, const std::wstring filePath = L"pandoraGS.lang"s)
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

        /// @brief Read values from file (error if not found)
        /// @param[in] filePath  Language file path
        /// @throws invalid_argument  File not found or not accessible
        virtual void setLanguageFromFile(const std::wstring& filePath) override;
    };
}
