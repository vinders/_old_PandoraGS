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
    profile.screenTab = L"Screen stretching"s;
    profile.compatibilityTab = L"Compatibility settings"s;

    generalSettings.groupDisplay = L"Display settings"s;
    generalSettings.resolution = L"Display resolution"s;
    generalSettings.fullscreenRes = L"fullscreen"s;
    generalSettings.windowRes = L"window"s;
    generalSettings.desktopRes = L"desktop resolution"s;
    generalSettings.resizable = L"Resizable window"s;
    generalSettings.colorDepth = L"Color depth"s;
    generalSettings.subprecision = L"GTE subprecision (anti-jitter)"s;
    generalSettings.noScreenSaver = L"Disable screensaver"s;
    generalSettings.groupTimer = L"Frame rate settings"s;
    generalSettings.timerLimit = L"Use frame rate limit"s;
    generalSettings.timerLimitAuto = L"auto-detect"s;
    generalSettings.timerLimitFixed = L"fixed (10-200)"s;
    generalSettings.timerSkipping = L"Use frame skipping"s;
    generalSettings.btnKeyBinding = L"Control key bindings"s;
    generalSettings.btnAdvanced = L"Advanced settings"s;

    keyBindingSettings.keyTitle0 = L"Profile menu"s;
    keyBindingSettings.keyDescription0 = L"Show/hide profile selection menu."s;
    keyBindingSettings.keyTitle1 = L"Previous profile"s;
    keyBindingSettings.keyDescription1 = L"Choose previous profile in menu."s;
    keyBindingSettings.keyTitle2 = L"Next profile"s;
    keyBindingSettings.keyDescription2 = L"Choose next profile in menu."s;
    keyBindingSettings.keyTitle3 = L"Default profile"s;
    keyBindingSettings.keyDescription3 = L"Choose default profile (0) in menu.";
    keyBindingSettings.keyTitle4 = L"Apply profile"s;
    keyBindingSettings.keyDescription4 = L"Apply selected profile (selected in menu)."s;
    keyBindingSettings.keyTitle5 = L"Toggle ratio"s;
    keyBindingSettings.keyDescription5 = L"Enable/disable PC pixel ratio conversion."s;
    keyBindingSettings.keyTitle6 = L"Fast forward"s;
    keyBindingSettings.keyDescription6 = L"Super-speed while the key is pressed."s;
    keyBindingSettings.keyTitle7 = L"Slow motion"s;
    keyBindingSettings.keyDescription7 = L"Enable/disable slow motion mode."s;
    keyBindingSettings.keyTitle8 = L"Pause"s;
    keyBindingSettings.keyDescription8 = L"Pause/resume game."s;
    keyBindingSettings.keyTitle9 = L"FPS display"s;
    keyBindingSettings.keyDescription9 = L"Show/hide frames per second."s;
    keyBindingSettings.backspace = L"<Backspace> : Toggle window mode"s;
    
    profileManager.groupList = L"Profile list management"s;
    //...

    filteringSettings.groupSmoothing = L"Smoothing / upscaling"s;
    //...
    filteringSettings.groupRendering = L"Rendering"s;
    //...
    filteringSettings.groupHdr = L"High dynamic range"s;
    //...
    filteringSettings.groupMisc = L"Miscellaneous"s;
    //...

    compatibilitySettings.groupColor = L"Color correction"s;
    //...
    compatibilitySettings.groupCompatibility = L"Compatibility"s;
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
    profile.screenTab = L"Étirement et ratio"s;
    profile.compatibilityTab = L"Options de compatibilité"s;
    
    generalSettings.groupDisplay = L"Options d'affichage"s;
    generalSettings.resolution = L"Résolution d'affichage"s;
    generalSettings.fullscreenRes = L"plein écran"s;
    generalSettings.windowRes = L"fenêtre"s;
    generalSettings.desktopRes = L"résolution du bureau"s;
    generalSettings.resizable = L"Redimensionnable"s;
    generalSettings.colorDepth = L"Couleurs"s;
    generalSettings.subprecision = L"Anti-tremblements (précision GTE)"s;
    generalSettings.noScreenSaver = L"Désactiver économiseur d'écran"s;
    generalSettings.groupTimer = L"Fréquence d'affichage"s;
    generalSettings.timerLimit = L"Limiter le frame-rate"s;
    generalSettings.timerLimitAuto = L"automatique"s;
    generalSettings.timerLimitFixed = L"fixe (10-200)"s;
    generalSettings.timerSkipping = L"Activer le saut d'images"s;
    generalSettings.btnKeyBinding = L"Touches de contrôle"s;
    generalSettings.btnAdvanced = L"Options avancées"s;

    keyBindingSettings.keyTitle0 = L"Menu de profils"s;
    keyBindingSettings.keyDescription0 = L"Afficher/masquer menu de sélection de profils."s;
    keyBindingSettings.keyTitle1 = L"Profil précédent"s;
    keyBindingSettings.keyDescription1 = L"Profil précédent dans le menu."s;
    keyBindingSettings.keyTitle2 = L"Profil suivant"s;
    keyBindingSettings.keyDescription2 = L"Profil suivant dans le menu."s;
    keyBindingSettings.keyTitle3 = L"Profil par défaut"s;
    keyBindingSettings.keyDescription3 = L"Profil par défaut (0) dans le menu.";
    keyBindingSettings.keyTitle4 = L"Appliquer profil"s;
    keyBindingSettings.keyDescription4 = L"Appliquer profil actuel (sélection du menu)."s;
    keyBindingSettings.keyTitle5 = L"Changer de ratio"s;
    keyBindingSettings.keyDescription5 = L"Activer/désactiver conversion en pixel ratio de PC."s;
    keyBindingSettings.keyTitle6 = L"Accélérer"s;
    keyBindingSettings.keyDescription6 = L"Super-vitesse tant que la touche est pressée."s;
    keyBindingSettings.keyTitle7 = L"Ralentir"s;
    keyBindingSettings.keyDescription7 = L"Activer/désactiver mode ralenti."s;
    keyBindingSettings.keyTitle8 = L"Pause"s;
    keyBindingSettings.keyDescription8 = L"Pause/reprise du jeu."s;
    keyBindingSettings.keyTitle9 = L"Affichage fréquence"s;
    keyBindingSettings.keyDescription9 = L"Afficher/masquer nombre d'images par seconde."s;
    keyBindingSettings.backspace = L"<Backspace> : fenêtre/plein écran"s;

    profileManager.groupList = L"Gestionnaire de profils"s;
    //...

    filteringSettings.groupSmoothing = L"Lissage / agrandissement"s;
    //...
    filteringSettings.groupRendering = L"Rendu"s;
    //...
    filteringSettings.groupHdr = L"Gamme dynamique (HDR)"s;
    //...
    filteringSettings.groupMisc = L"Divers"s;
    //...

    compatibilitySettings.groupColor = L"Correction des couleurs"s;
    //...
    compatibilitySettings.groupCompatibility = L"Compatibilité"s;
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