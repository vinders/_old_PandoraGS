/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_page.h
Description : configuration dialog page - business logic
*******************************************************************************/
#ifndef _CONFIG_PAGE_H
#define _CONFIG_PAGE_H
#include "globals.h"

#ifndef _CONFIG_DIALOG_H_END
class ConfigDialog;
#endif
#include "config_io.h"
#include "config_page_qtview.h"
#include "config_page_winview.h"
#include "lang.h"


// Configuration dialog page (generic) - business logic
class ConfigPage
{
protected:
    ConfigDialog* m_pParent; // parent controller reference
    ConfigPageView* m_pView; // visuals container

public:
    /// <summary>Initialize controller variables</summary>
    /// <param name="pParent">Parent controller reference</param>
    ConfigPage(ConfigDialog* pParent);
    /// <summary>Destroy controller data</summary>
    virtual ~ConfigPage() {}

    /// <summary>Create config page view</summary>
    /// <exception cref="std::exception">Creation failure</exception>
    virtual void createView() = 0;

    /// <summary>Get page view</summary>
    /// <returns>Page view pointer</returns>
    ConfigPageView* getView() const
    {
        return m_pView;
    }
    /// <summary>Get language resource</summary>
    /// <returns>Language resource pointer</returns>
    LanguageDialogResource* getLangResource();
};

#define _CONFIG_PAGE_H_END
#include "config_dialog.h"

#endif
