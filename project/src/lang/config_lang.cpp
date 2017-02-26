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
    profile.profileList = L"Current profile"s;
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
    profileManager.btnAdd = L"Add a new profile";
    profileManager.btnEdit = L"Edit the selected profile (name and associations)";
    profileManager.btnRemove = L"Remove selected profiles";
    profileManager.btnImport = L"Import profile (from file)";
    profileManager.btnExport = L"Export profile (save file)";
    profileManager.profilePresets = L"Profile presets"s;
    profileManager.btnPresets = L"Apply to selection"s;
    profileManager.tableNumber = L"#"s;
    profileManager.tableProfile = L"profile"s;
    profileManager.presets = { L"Fastest"s, L"Standard"s, L"Enhanced 2D"s, L"Enhanced 3D"s };
    profileManager.msgBoxRemoveEmpty = L"Please select at least one profile to remove."s;
    profileManager.msgBoxRemoveEmptyTitle = L"No profile selected..."s;
    profileManager.msgBoxRemoveDefault = L"The default profile can't be removed."s;
    profileManager.msgBoxRemoveDefaultTitle = L"Removal not allowed..."s;
    profileManager.msgBoxRemoveConfirm = L"Are you sure you want to remove the selected profiles?"s;
    profileManager.msgBoxRemoveConfirmTitle = L"Confirm removal..."s;

    filteringSettings.groupSmoothing = L"Smoothing / upscaling"s;
    filteringSettings.nativeScale = L"native"s;
    filteringSettings.noScreenScale = L"normal"s;
    filteringSettings.interpolations = { L"Nearest neighbor", L"Bilinear filter - standard", L"Bilinear filter - enhanced", L"Bicubic filter (slow)", L"Gaussian filter (slow)" };
    filteringSettings.screenSmoothing = { L"None", L"Slight blur", L"Blur", L"Blur & add grain" };
    //...
    filteringSettings.groupRendering = L"Rendering"s;
    //...
    filteringSettings.groupHdr = L"High dynamic range"s;
    //...
    filteringSettings.groupMisc = L"Miscellaneous"s;
    //...
    filteringSettings.btnScanlines = L"Scanlines settings..."s;
    filteringSettings.btnCursor = L"Cursor settings..."s;

    screenSettings.internalRes = L"Internal resolution"s;
    screenSettings.internalResX = { L"X: native (1x)"s, L"X: standard (2x)"s, L"X: high (4x)"s, L"X: ultra (6x, slow)"s, L"X: ultra+ (8x, slow)"s };
    screenSettings.internalResY = { L"Y: native (1x)"s, L"Y: low (2x)"s, L"Y: standard (4x)"s, L"Y: high (8x)"s, L"Y: ultra (10x, slow)"s, L"Y: ultra+ (12x, slow)"s };
    screenSettings.stretchingPresets = { L"Custom settings"s, L"Full window (stretched)"s, L"Keep ratio (best with 2D)"s, L"Keep ratio & fill (cropped)"s, L"Semi stretched/cropped"s };
    screenSettings.unstretched = L"Keep ratio"s;
    screenSettings.stretched = L"stretch"s;
    screenSettings.uncropped = L"Uncropped"s;
    screenSettings.cropped = L"fill/crop"s;
    screenSettings.pixelRatio = L"Non-square pixel ratio (for CRT screens)"s;
    screenSettings.mirror = L"Mirror screen"s;
    screenSettings.blackBorders = L"Add black borders (anti-flicker):"s;
    screenSettings.blackBordersX = L"X (sides):"s;
    screenSettings.blackBordersY = L"Y (top/bottom):"s;
    //...

    compatibilitySettings.groupColor = L"Color correction"s;
    //...
    compatibilitySettings.groupCompatibility = L"Compatibility"s;
    //...
    compatibilitySettings.btnGameFixes = L"Custom game fixes..."s;
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
    profile.profileList = L"Profil courant"s;
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
    profileManager.btnAdd = L"Ajouter un profil";
    profileManager.btnEdit = L"Editer le profil sélectionné (nom et associations)";
    profileManager.btnRemove = L"Supprimer la sélection";
    profileManager.btnImport = L"Importer profil (depuis fichier)";
    profileManager.btnExport = L"Exporter profil (sauvegarder)";
    profileManager.profilePresets = L"Préconfigurer profil"s;
    profileManager.btnPresets = L"Appliquer à la sélection"s;
    profileManager.tableNumber = L"#"s;
    profileManager.tableProfile = L"profil"s;
    profileManager.presets = { L"Le plus rapide"s, L"Standard"s, L"2D optimisée"s, L"3D optimisée"s };
    profileManager.msgBoxRemoveEmpty = L"Veuillez choisir au moins un profil à supprimer."s;
    profileManager.msgBoxRemoveEmptyTitle = L"Aucun profil sélectionné..."s;
    profileManager.msgBoxRemoveDefault = L"Le profil par défaut ne peut pas être supprimé."s;
    profileManager.msgBoxRemoveDefaultTitle = L"Suppression impossible..."s;
    profileManager.msgBoxRemoveConfirm = L"Voulez-vous supprimer les profils sélectionnés ?"s;
    profileManager.msgBoxRemoveConfirmTitle = L"Confirmer la suppression..."s;

    filteringSettings.groupSmoothing = L"Lissage / agrandissement"s;
    filteringSettings.nativeScale = L"natif"s;
    filteringSettings.noScreenScale = L"normal"s;
    filteringSettings.interpolations = { L"Au plus proche", L"Filtre bilinéaire - standard", L"Filtre bilinéaire - amélioré", L"Filtre bicubique (lent)", L"Filtre gaussien (lent)" };
    filteringSettings.screenSmoothing = { L"Aucun", L"Léger flou", L"Flou", L"Flou & ajout de bruit" };
    //...
    filteringSettings.groupRendering = L"Rendu"s;
    //...
    filteringSettings.groupHdr = L"Gamme dynamique (HDR)"s;
    //...
    filteringSettings.groupMisc = L"Divers"s;
    //...
    filteringSettings.btnScanlines = L"Options des lignes..."s;
    filteringSettings.btnCursor = L"Options du viseur..."s;

    screenSettings.internalRes = L"Résolution interne"s;
    screenSettings.internalResX = { L"X: native (1x)"s, L"X: standard (2x)"s, L"X: haute (4x)"s, L"X: ultra (6x, lent)"s, L"X: ultra+ (8x, lent)"s };
    screenSettings.internalResY = { L"Y: native (1x)"s, L"Y: basse (2x)"s, L"Y: standard (4x)"s, L"Y: haute (8x)"s, L"Y: ultra (10x, lent)"s, L"Y: ultra+ (12x, lent)"s };
    screenSettings.stretchingPresets = { L"Personnalisé"s, L"Remplir la fenêtre (étirer)"s, L"Conserver ratio (idéal en 2D)"s, L"Conserver ratio & tronquer"s, L"Semi étiré/tronqué"s };
    screenSettings.unstretched = L"Garder ratio"s;
    screenSettings.stretched = L"étirer"s;
    screenSettings.uncropped = L"Non tronqué"s;
    screenSettings.cropped = L"remplir"s;
    screenSettings.pixelRatio = L"Pixels non carrés (écrans cathodiques)"s;
    screenSettings.mirror = L"Inverser l'écran"s;
    screenSettings.blackBorders = L"Bordures noires (anti-clignotements) :"s;
    screenSettings.blackBordersX = L"X (côtés) :"s;
    screenSettings.blackBordersY = L"Y (haut/bas) :"s;
    //...

    compatibilitySettings.groupColor = L"Correction des couleurs"s;
    //...
    compatibilitySettings.groupCompatibility = L"Compatibilité"s;
    //...
    compatibilitySettings.btnGameFixes = L"Corrections spécifiques..."s;
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