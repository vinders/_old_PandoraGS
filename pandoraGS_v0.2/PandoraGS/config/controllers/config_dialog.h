/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_dialog.h
Description : configuration dialog - business logic
*******************************************************************************/
#ifndef _CONFIG_DIALOG_H
#define _CONFIG_DIALOG_H
#include "globals.h"
#include "lang.h"
#include "config_io.h"
#include "config_page.h"
#include "config_page_general.h"
#include "config_page_manager.h"
#include "config_page_profile.h"
#include "config_dialog_winview.h"
#include "config_dialog_qtview.h"
#ifndef _CONFIG_DIALOG_VIEW_H_END
class ConfigDialogView;
#endif

#define CONFIG_DIALOG_PAGES_NB 3u
#define CONFIG_PAGE_GENERAL 0u
#define CONFIG_PAGE_MANAGER 1u
#define CONFIG_PAGE_PROFILE 2u


// Configuration dialog box - business logic
class ConfigDialog
{
private:
    ConfigDialogView* m_pView; // visuals container
    LanguageDialogResource* m_pLang; // language resource
    ConfigPage** m_pPages; // dialog pages

public:
    /// <summary>Initialize controller variables</summary>
    ConfigDialog();
    /// <summary>Destroy controller data</summary>
    ~ConfigDialog();

    /// <summary>Create and display config dialog</summary>
    void showDialog();

    // -- GETTERS -- -----------------------------------------------------------

    /// <summary>Get language resource</summary>
    /// <returns>Language resource pointer</returns>
    inline LanguageDialogResource* getLangResource()
    {
        return m_pLang;
    }
    /// <summary>Get subpage controller</summary>
    /// <param name="index">Page identifier</param>
    /// <returns>Page controller pointer</returns>
    inline ConfigPage* getConfigPage(uint32_t index)
    {
        return m_pPages[index];
    }
};

#define _CONFIG_DIALOG_H_END

#endif
