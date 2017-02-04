/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   lang.h
Description : language resources
*******************************************************************************/
#ifndef _LANG_H
#define _LANG_H

#include <stdint.h>
#include <string>

// data types
enum langcode_t : uint32_t // languages
{
    Langcode_english = 0,
    Langcode_spanish = 1,
    Langcode_french = 2,
    Langcode_german = 3,
    Langcode_customFile = 1000 // external file
};
#define LANG_DEFAULT Langcode_english
#define LANG_LAST_INTERNAL Langcode_german
#define LANG_NAMES_LIST {L" English",L" Español",L" Français",L" Deutsch",L" External file..."}


// Ingame menu language resource (static)
class LanguageGameMenuResource
{
public:
    //static std::string ...

public:
    /// <summary>Set game language values (necessary)</summary>
    /// <param name="code">Language code</param>
    /// <param name="filePath">Language file path</param>
    static void setLanguage(langcode_t code, std::wstring filePath = L"pandoraGS.lang");
    /// <summary>Set english values</summary>
    static void setLanguage_English();
    /// <summary>Set spanish values</summary>
    static void setLanguage_Spanish();
    /// <summary>Set french values</summary>
    static void setLanguage_French();
    /// <summary>Set german values</summary>
    static void setLanguage_German();
    /// <summary>Read values from file (english if not found)</summary>
    /// <param name="filePath">Language file path (optional)</param>
    static void setLanguageFromFile(std::wstring filePath);
};


// Dialog language resource
class LanguageDialogResource
{
public:
    std::wstring dialog_ok;
    std::wstring dialog_cancel;
    std::wstring dialog_profiles;
    std::wstring tabTitle_generalSettings;
    std::wstring tabTitle_profilesManager;
    std::wstring tabTitle_profileSettings;

    std::wstring manager_tableColumnProfile;
    std::wstring manager_btnAdd_tooltip;
    std::wstring manager_btnEdit_tooltip;
    std::wstring manager_btnRemove_tooltip;
    std::wstring manager_btnImport_tooltip;
    std::wstring manager_btnExport_tooltip;
    std::wstring manager_presets;
    std::wstring manager_btnPresetsApply;
    std::wstring manager_preset_fastest;
    std::wstring manager_preset_standard;
    std::wstring manager_preset_enhanced2d;
    std::wstring manager_preset_enhanced3d;

#define LANG_PROFILE_INTERPOLATION 5
#define LANG_PROFILE_SCREENSMOOTH 4
#define LANG_PROFILE_AA 6
    std::wstring profile_interpolation[LANG_PROFILE_INTERPOLATION];
    std::wstring profile_screenSmooth[LANG_PROFILE_SCREENSMOOTH];
    std::wstring profile_antialiasing[LANG_PROFILE_AA];
#define LANG_PROFILE_INTERNAL_RESX 5
#define LANG_PROFILE_INTERNAL_RESY 6
#define LANG_PROFILE_STRETCH_PRESETS 5
    std::wstring profile_internal_resx[LANG_PROFILE_INTERNAL_RESX];
    std::wstring profile_internal_resy[LANG_PROFILE_INTERNAL_RESY];
    std::wstring profile_stretch_presets[LANG_PROFILE_STRETCH_PRESETS];

public:
    /// <summary>Create uninitialized instance</summary>
    LanguageDialogResource() {}

    /// <summary>Set dialog language values (necessary)</summary>
    /// <param name="code">Language code</param>
    /// <param name="filePath">Language file path (optional)</param>
    void setLanguage(langcode_t code, std::wstring filePath = L"pandoraGS.lang");
    /// <summary>Set english values</summary>
    void setLanguage_English();
    /// <summary>Set spanish values</summary>
    void setLanguage_Spanish();
    /// <summary>Set french values</summary>
    void setLanguage_French();
    /// <summary>Set german values</summary>
    void setLanguage_German();
    /// <summary>Read values from file</summary>
    /// <param name="filePath">Language file path</param>
    /// <exception cref="std::exception">File not found</exception>
    void setLanguageFromFile(std::wstring filePath);
};

#endif
