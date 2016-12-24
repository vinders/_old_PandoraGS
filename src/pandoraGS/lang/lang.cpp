/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   lang.h
Description : language resources
*******************************************************************************/
#include <iostream>
#include <fstream>
using namespace std;
#include "lang.h"
#include "logger.h"


// -- INGAME MENU LANGUAGE RESOURCE -- -----------------------------------------

/// <summary>Set game language values (necessary)</summary>
/// <param name="code">Language code</param>
/// <param name="filePath">Language file path (optional)</param>
void LanguageGameMenuResource::setLanguage(langcode_t code, std::wstring filePath)
{
    // language factory
    switch (code)
    {
        case Langcode_english: setLanguage_English(); break;
        case Langcode_spanish: setLanguage_Spanish(); break;
        case Langcode_french:  setLanguage_French();  break;
        case Langcode_german:  setLanguage_German();  break;
        case Langcode_customFile: setLanguageFromFile(filePath); break;
        default: setLanguage_English(); break;
    }
}

/// <summary>Set english values</summary>
void LanguageGameMenuResource::setLanguage_English()
{
    
}
/// <summary>Set spanish values</summary>
void LanguageGameMenuResource::setLanguage_Spanish()
{

}
/// <summary>Set french values</summary>
void LanguageGameMenuResource::setLanguage_French()
{

}
/// <summary>Set german values</summary>
void LanguageGameMenuResource::setLanguage_German()
{

}

/// <summary>Read values from file (english if not found)</summary>
/// <param name="filePath">Language file path</param>
void LanguageGameMenuResource::setLanguageFromFile(std::wstring filePath)
{
    try
    {
        setLanguage_English(); // default values

        // open language file
        std::ifstream in;
        in.open(filePath, std::ifstream::in);
        if (!in.is_open())
        {
            Logger::getInstance()->writeErrorEntry("LanguageGameMenuResource.setLanguageFromFile", "File not found or not accessible.");
            return;
        }

        //...read file

        in.close();
    }
    catch (const exception& exc) // ingame -> no exception
    {
        Logger::getInstance()->writeErrorEntry("LanguageGameMenuResource.setLanguageFromFile", exc.what());
    }
}


// -- DIALOG LANGUAGE RESOURCE -- ----------------------------------------------

/// <summary>Set dialog language values (necessary)</summary>
/// <param name="code">Language code</param>
/// <param name="filePath">Language file path (optional)</param>
void LanguageDialogResource::setLanguage(langcode_t code, std::wstring filePath)
{
    // language factory
    switch (code)
    {
        case Langcode_english: setLanguage_English(); break;
        case Langcode_spanish: setLanguage_Spanish(); break;
        case Langcode_french:  setLanguage_French();  break;
        case Langcode_german:  setLanguage_German();  break;
        case Langcode_customFile: setLanguageFromFile(filePath); break;
        default: setLanguage_English(); break;
    }
}

/// <summary>Set english values</summary>
void LanguageDialogResource::setLanguage_English()
{
    dialog_ok = L"OK";
    dialog_cancel = L"Cancel";
    dialog_profiles = L"Current profile:";
    tabTitle_generalSettings = L"General settings";
    tabTitle_profilesManager = L"Profile manager";
    tabTitle_profileSettings = L"Profile settings";

    manager_tableColumnProfile = L"profile";
    manager_btnAdd_tooltip = L"Add a new profile";
    manager_btnEdit_tooltip = L"Edit the selected profile (name and associations)";
    manager_btnRemove_tooltip = L"Remove selected profiles";
    manager_btnImport_tooltip = L"Import profile (from file)";
    manager_btnExport_tooltip = L"Export profile (save file)";
    manager_presets = L"Profile presets";
    manager_btnPresetsApply = L"Apply to selection";
    manager_preset_fastest = L"Fastest";
    manager_preset_standard = L"Standard";
    manager_preset_enhanced2d = L"Enhanced 2D";
    manager_preset_enhanced3d = L"Enhanced 3D";

    profile_interpolation[0] = L"None - nearest neighbor";
    profile_interpolation[1] = L"Standard - bilinear";
    profile_interpolation[2] = L"Enhanced - bilinear";
    profile_interpolation[3] = L"Bicubic (slow)";
    profile_interpolation[4] = L"Gaussian (slow)";
    profile_screenSmooth[0] = L"None";
    profile_screenSmooth[1] = L"Slight blur";
    profile_screenSmooth[2] = L"Blur";
    profile_screenSmooth[3] = L"Blur & add grain";
    profile_antialiasing[0] = L"FXAA (fast, slight blur)";
    profile_antialiasing[1] = L"NFAA (subtle, 3D only)";
    profile_antialiasing[2] = L"SMAA 4x (fast & nice)";
    profile_antialiasing[3] = L"SMAA 8x (fast & nice)";
    profile_antialiasing[4] = L"MSAA 4x (slow)";
    profile_antialiasing[5] = L"MSAA 8x (very slow)";

    profile_internal_resx[0] = L"X: native (1x)";
    profile_internal_resx[1] = L"X: standard (2x)";
    profile_internal_resx[2] = L"X: high (4x)";
    profile_internal_resx[3] = L"X: higher (6x, slow)";
    profile_internal_resx[4] = L"X: ultra (8x, slow)";
    profile_internal_resy[0] = L"Y: native (1x)";
    profile_internal_resy[1] = L"Y: standard (2x)";
    profile_internal_resy[2] = L"Y: medium (4x)";
    profile_internal_resy[3] = L"Y: high (8x)";
    profile_internal_resy[4] = L"Y: higher (10x, slow)";
    profile_internal_resy[5] = L"Y: ultra (12x, slow)";
    profile_stretch_presets[0] = L"Custom settings";
    profile_stretch_presets[1] = L"Full window (stretched)";
    profile_stretch_presets[2] = L"Keep ratio (best with 2D)";
    profile_stretch_presets[3] = L"Keep ratio & fill (cropped)";
    profile_stretch_presets[4] = L"Semi stretched/cropped";
}
/// <summary>Set spanish values</summary>
void LanguageDialogResource::setLanguage_Spanish()
{
    dialog_ok = L"OK";
    dialog_cancel = L"Cancelar";
    dialog_profiles = L"Current profile:";
    tabTitle_generalSettings = L"General settings";
    tabTitle_profilesManager = L"Profile manager";
    tabTitle_profileSettings = L"Profile settings";

    manager_tableColumnProfile = L"profile";
    manager_btnAdd_tooltip = L"Add a new profile";
    manager_btnEdit_tooltip = L"Edit the selected profile (name and associations)";
    manager_btnRemove_tooltip = L"Remove selected profiles";
    manager_btnImport_tooltip = L"Import profile (from file)";
    manager_btnExport_tooltip = L"Export profile (save file)";
    manager_presets = L"Profile presets";
    manager_btnPresetsApply = L"Apply to selection";
    manager_preset_fastest = L"Fastest";
    manager_preset_standard = L"Standard";
    manager_preset_enhanced2d = L"Enhanced 2D";
    manager_preset_enhanced3d = L"Enhanced 3D";

    profile_interpolation[0] = L"None - nearest neighbor";
    profile_interpolation[1] = L"Standard - bilinear";
    profile_interpolation[2] = L"Enhanced - bilinear";
    profile_interpolation[3] = L"Bicubic (slow)";
    profile_interpolation[4] = L"Gaussian (slow)";
    profile_screenSmooth[0] = L"None";
    profile_screenSmooth[1] = L"Slight blur";
    profile_screenSmooth[2] = L"Blur";
    profile_screenSmooth[3] = L"Blur & add grain";
    profile_antialiasing[0] = L"FXAA (fast, slight blur)";
    profile_antialiasing[1] = L"NFAA (subtle, 3D only)";
    profile_antialiasing[2] = L"SMAA 4x (fast & nice)";
    profile_antialiasing[3] = L"SMAA 8x (fast & nice)";
    profile_antialiasing[4] = L"MSAA 4x (slow)";
    profile_antialiasing[5] = L"MSAA 8x (very slow)";

    profile_internal_resx[0] = L"X: native (1x)";
    profile_internal_resx[1] = L"X: standard (2x)";
    profile_internal_resx[2] = L"X: high (4x)";
    profile_internal_resx[3] = L"X: higher (6x, slow)";
    profile_internal_resx[4] = L"X: ultra (8x, slow)";
    profile_internal_resy[0] = L"Y: native (1x)";
    profile_internal_resy[1] = L"Y: standard (2x)";
    profile_internal_resy[2] = L"Y: medium (4x)";
    profile_internal_resy[3] = L"Y: high (8x)";
    profile_internal_resy[4] = L"Y: higher (10x, slow)";
    profile_internal_resy[5] = L"Y: ultra (12x, slow)";
    profile_stretch_presets[0] = L"Custom settings";
    profile_stretch_presets[1] = L"Full window (stretched)";
    profile_stretch_presets[2] = L"Keep ratio (best with 2D)";
    profile_stretch_presets[3] = L"Keep ratio & fill (cropped)";
    profile_stretch_presets[4] = L"Semi stretched/cropped";
}
/// <summary>Set french values</summary>
void LanguageDialogResource::setLanguage_French()
{
    dialog_ok = L"OK";
    dialog_cancel = L"Annuler";
    dialog_profiles = L"Profil courant :";
    tabTitle_generalSettings = L"Options générales";
    tabTitle_profilesManager = L"Gestion de profils";
    tabTitle_profileSettings = L"Paramétrer profil";

    manager_tableColumnProfile = L"profil";
    manager_btnAdd_tooltip = L"Ajouter un profil";
    manager_btnEdit_tooltip = L"Editer le profil sélectionné (nom et associations)";
    manager_btnRemove_tooltip = L"Supprimer la sélection";
    manager_btnImport_tooltip = L"Importer profil (depuis fichier)";
    manager_btnExport_tooltip = L"Exporter profil (sauvegarder)";
    manager_presets = L"Préconfigurer profil";
    manager_btnPresetsApply = L"Modifier sélection";
    manager_preset_fastest = L"Plus rapide";
    manager_preset_standard = L"Standard";
    manager_preset_enhanced2d = L"2D améliorée";
    manager_preset_enhanced3d = L"3D améliorée";

    profile_interpolation[0] = L"Aucun - au plus proche";
    profile_interpolation[1] = L"Standard - bilinéaire";
    profile_interpolation[2] = L"Amélioré - bilinéaire";
    profile_interpolation[3] = L"Bicubique (lent)";
    profile_interpolation[4] = L"Gaussien (lent)";
    profile_screenSmooth[0] = L"Aucun";
    profile_screenSmooth[1] = L"Léger flou";
    profile_screenSmooth[2] = L"Flou";
    profile_screenSmooth[3] = L"Flou & ajout de grain";
    profile_antialiasing[0] = L"FXAA (rapide, léger flou)";
    profile_antialiasing[1] = L"NFAA (subtil, pour 3D)";
    profile_antialiasing[2] = L"SMAA 4x (rapide & bon)";
    profile_antialiasing[3] = L"SMAA 8x (rapide & bon)";
    profile_antialiasing[4] = L"MSAA 4x (lent)";
    profile_antialiasing[5] = L"MSAA 8x (très lent)";

    profile_internal_resx[0] = L"X: native (1x)";
    profile_internal_resx[1] = L"X: standard (2x)";
    profile_internal_resx[2] = L"X: haute (4x)";
    profile_internal_resx[3] = L"X: haute+ (6x, lent)";
    profile_internal_resx[4] = L"X: ultra (8x, lent)";
    profile_internal_resy[0] = L"Y: native (1x)";
    profile_internal_resy[1] = L"Y: standard (2x)";
    profile_internal_resy[2] = L"Y: medium (4x)";
    profile_internal_resy[3] = L"Y: haute (8x)";
    profile_internal_resy[4] = L"Y: haute+ (10x, lent)";
    profile_internal_resy[5] = L"Y: ultra (12x, lent)";
    profile_stretch_presets[0] = L"Personnalisé";
    profile_stretch_presets[1] = L"Remplir fenêtre (étirer)";
    profile_stretch_presets[2] = L"Conserver ratio (idéal 2D)";
    profile_stretch_presets[3] = L"Conserver ratio & tronquer";
    profile_stretch_presets[4] = L"Semi étiré/tronqué";
}
/// <summary>Set german values</summary>
void LanguageDialogResource::setLanguage_German()
{
    dialog_ok = L"OK";
    dialog_cancel = L"Abbrechen";
    dialog_profiles = L"Current profile:";
    tabTitle_generalSettings = L"General settings";
    tabTitle_profilesManager = L"Profile manager";
    tabTitle_profileSettings = L"Profile settings";

    manager_tableColumnProfile = L"profile";
    manager_btnAdd_tooltip = L"Add a new profile";
    manager_btnEdit_tooltip = L"Edit the selected profile (name and associations)";
    manager_btnRemove_tooltip = L"Remove selected profiles";
    manager_btnImport_tooltip = L"Import profile (from file)";
    manager_btnExport_tooltip = L"Export profile (save file)";
    manager_presets = L"Profile presets";
    manager_btnPresetsApply = L"Apply to selection";
    manager_preset_fastest = L"Fastest";
    manager_preset_standard = L"Standard";
    manager_preset_enhanced2d = L"Enhanced 2D";
    manager_preset_enhanced3d = L"Enhanced 3D";

    profile_interpolation[0] = L"None - nearest neighbor";
    profile_interpolation[1] = L"Standard - bilinear";
    profile_interpolation[2] = L"Enhanced - bilinear";
    profile_interpolation[3] = L"Bicubic (slow)";
    profile_interpolation[4] = L"Gaussian (slow)";
    profile_screenSmooth[0] = L"None";
    profile_screenSmooth[1] = L"Slight blur";
    profile_screenSmooth[2] = L"Blur";
    profile_screenSmooth[3] = L"Blur & add grain";
    profile_antialiasing[0] = L"FXAA (fast, slight blur)";
    profile_antialiasing[1] = L"NFAA (subtle, 3D only)";
    profile_antialiasing[2] = L"SMAA 4x (fast & nice)";
    profile_antialiasing[3] = L"SMAA 8x (fast & nice)";
    profile_antialiasing[4] = L"MSAA 4x (slow)";
    profile_antialiasing[5] = L"MSAA 8x (very slow)";

    profile_internal_resx[0] = L"X: native (1x)";
    profile_internal_resx[1] = L"X: standard (2x)";
    profile_internal_resx[2] = L"X: high (4x)";
    profile_internal_resx[3] = L"X: higher (6x, slow)";
    profile_internal_resx[4] = L"X: ultra (8x, slow)";
    profile_internal_resy[0] = L"Y: native (1x)";
    profile_internal_resy[1] = L"Y: standard (2x)";
    profile_internal_resy[2] = L"Y: medium (4x)";
    profile_internal_resy[3] = L"Y: high (8x)";
    profile_internal_resy[4] = L"Y: higher (10x, slow)";
    profile_internal_resy[5] = L"Y: ultra (12x, slow)";
    profile_stretch_presets[0] = L"Custom settings";
    profile_stretch_presets[1] = L"Full window (stretched)";
    profile_stretch_presets[2] = L"Keep ratio (best with 2D)";
    profile_stretch_presets[3] = L"Keep ratio & fill (cropped)";
    profile_stretch_presets[4] = L"Semi stretched/cropped";
}

/// <summary>Read values from file</summary>
/// <param name="filePath">Language file path</param>
/// <exception cref="std::exception">File not found</exception>
void LanguageDialogResource::setLanguageFromFile(std::wstring filePath)
{
    setLanguage_English(); // default values

    // open language file
    std::ifstream in;
    in.open(filePath, std::ifstream::in); // overwrite
    if (!in.is_open())
        throw std::exception("File not found or not accessible.");

    //...read file

    in.close();
}
