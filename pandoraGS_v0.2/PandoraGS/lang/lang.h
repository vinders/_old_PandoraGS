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

#include <string>

// languages
enum LangCode
{
    LangCode_English = 0,
    LangCode_Spanish = 1,
    LangCode_French = 2,
    LangCode_German = 3,
    LangCode_CustomFile = 1000 // external file 'pandoraGS.lang'
};
#define LANG_DEFAULT LangCode_English


// Ingame menu language resource (static)
class LanguageGameMenuResource
{
public:
    //static std::string ...

public:
    /// <summary>Set game language values (necessary)</summary>
    /// <param name="code">Language code</param>
    static void setLanguage(LangCode code);
    /// <summary>Set english values</summary>
    static void setLanguage_English();
    /// <summary>Set spanish values</summary>
    static void setLanguage_Spanish();
    /// <summary>Set french values</summary>
    static void setLanguage_French();
    /// <summary>Set german values</summary>
    static void setLanguage_German();
    /// <summary>Read values from file (english if not found)</summary>
    static void setLanguageFromFile();
};


// Dialog language resource
class LanguageDialogResource
{
public:
    std::wstring dialog_ok;
    std::wstring dialog_cancel;
    std::wstring tabTitle_generalSettings;
    std::wstring tabTitle_profilesManager;
    std::wstring tabTitle_profileSettings;

public:
    /// <summary>Create uninitialized instance</summary>
    LanguageDialogResource();

    /// <summary>Set dialog language values (necessary)</summary>
    /// <param name="code">Language code</param>
    void setLanguage(LangCode code);
    /// <summary>Set english values</summary>
    void setLanguage_English();
    /// <summary>Set spanish values</summary>
    void setLanguage_Spanish();
    /// <summary>Set french values</summary>
    void setLanguage_French();
    /// <summary>Set german values</summary>
    void setLanguage_German();
    /// <summary>Read values from file</summary>
    /// <exception cref="std::exception">File not found</exception>
    void setLanguageFromFile();
};

#endif
