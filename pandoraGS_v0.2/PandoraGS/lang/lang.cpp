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
    tabTitle_generalSettings = L"General settings";
    tabTitle_profilesManager = L"Profile manager";
    tabTitle_profileSettings = L"Profile settings";
}
/// <summary>Set spanish values</summary>
void LanguageDialogResource::setLanguage_Spanish()
{
    dialog_ok = L"OK";
    dialog_cancel = L"Cancelar";
    tabTitle_generalSettings = L"General settings";
    tabTitle_profilesManager = L"Profile manager";
    tabTitle_profileSettings = L"Profile settings";
}
/// <summary>Set french values</summary>
void LanguageDialogResource::setLanguage_French()
{
    dialog_ok = L"OK";
    dialog_cancel = L"Annuler";
    tabTitle_generalSettings = L"Options générales";
    tabTitle_profilesManager = L"Gestion de profils";
    tabTitle_profileSettings = L"Paramétrer profil";
}
/// <summary>Set german values</summary>
void LanguageDialogResource::setLanguage_German()
{
    dialog_ok = L"OK";
    dialog_cancel = L"Abbrechen";
    tabTitle_generalSettings = L"General settings";
    tabTitle_profilesManager = L"Profile manager";
    tabTitle_profileSettings = L"Profile settings";
}

/// <summary>Read values from file</summary>
/// <exception cref="std::exception">File not found</exception>
void LanguageDialogResource::setLanguageFromFile()
{
    setLanguage_English();
    //...lire fichier
}
