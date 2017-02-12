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
using namespace std::literals::string_literals;
#include "config_lang.h"
using namespace lang;


/// @brief Set english values
void ConfigLang::setLanguageEnglish() noexcept
{
    dialog.confirm = L"OK"s;
    dialog.cancel  = L"Cancel"s;
    dialog.generalSettings   = L"General settings"s;
    dialog.profileManagement = L"Profile manager"s;
    dialog.profileSettings   = L"Profile settings"s;
    profile.profileList = L"Current profile:"s;
    profile.filteringTab = L"Filters"s;
    profile.screenTab = L"Display and ratio"s;
    profile.compatibilityTab = L"Compatibility settings"s;
    
    //...
}

/// @brief Set spanish values
void ConfigLang::setLanguageSpanish() noexcept
{
    /*...to do...*/setLanguageEnglish();
}

/// @brief Set french values
void ConfigLang::setLanguageFrench() noexcept
{
    dialog.confirm = L"OK"s;
    dialog.cancel  = L"Annuler"s;
    dialog.generalSettings   = L"Options générales"s;
    dialog.profileManagement = L"Gestion de profils"s;
    dialog.profileSettings   = L"Paramétrer profil"s;
    profile.profileList = L"Profil courant :"s;
    profile.filteringTab = L"Filtres"s;
    profile.screenTab = L"Affichage et ratio"s;
    profile.compatibilityTab = L"Options de compatibilité"s;
    
    //...
}

/// @brief Set german values
void ConfigLang::setLanguageGerman() noexcept
{
    /*...to do...*/setLanguageEnglish();
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