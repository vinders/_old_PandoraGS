/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   lang.h
Description : language resources
*******************************************************************************/
using namespace std;
#include "lang.h"


// -- INGAME MENU LANGUAGE RESOURCE -- -----------------------------------------

/// <summary>Set game language values (necessary)</summary>
/// <param name="code">Language code</param>
void LanguageGameMenuResource::setLanguage(LangCode code) 
{
    // language factory
    switch (code)
    {
        case LangCode_English: setLanguage_English(); break;
        case LangCode_Spanish: setLanguage_Spanish(); break;
        case LangCode_French:  setLanguage_French();  break;
        case LangCode_German:  setLanguage_German();  break;
        case LangCode_CustomFile: setLanguageFromFile(); break;
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
void LanguageGameMenuResource::setLanguageFromFile()
{

}


// -- DIALOG LANGUAGE RESOURCE -- ----------------------------------------------

/// <summary>Create uninitialized instance</summary>
LanguageDialogResource::LanguageDialogResource() 
{
}
/// <summary>Set dialog language values (necessary)</summary>
/// <param name="code">Language code</param>
void LanguageDialogResource::setLanguage(LangCode code) 
{
    // language factory
    switch (code)
    {
        case LangCode_English: setLanguage_English(); break;
        case LangCode_Spanish: setLanguage_Spanish(); break;
        case LangCode_French:  setLanguage_French();  break;
        case LangCode_German:  setLanguage_German();  break;
        case LangCode_CustomFile: setLanguageFromFile(); break;
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
}

/// <summary>Read values from file</summary>
/// <exception cref="std::exception">File not found</exception>
void LanguageDialogResource::setLanguageFromFile()
{
    setLanguage_English();
    //...lire fichier
}
