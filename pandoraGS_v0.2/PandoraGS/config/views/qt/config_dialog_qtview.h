/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_dialog_qtview.h
Description : configuration dialog - view
*******************************************************************************/
#ifndef _CONFIG_DIALOG_QTVIEW_H
#define _CONFIG_DIALOG_QTVIEW_H
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_QT

#ifndef _CONFIG_DIALOG_H_END
class ConfigDialog;
#endif
#include "config_page_qtview.h"


// Configuration dialog box - view
class ConfigDialogView
{
private:
    ConfigPageView* m_pPages; // sub-page views
    ConfigDialog* m_pController; // controller reference

public:
    /// <summary>Create dialog view container</summary>
    /// <param name="pController">Controller reference</param>
    ConfigDialogView(ConfigDialog* pController);
    /// <summary>Destroy dialog view container</summary>
    ~ConfigDialogView();

    /// <summary>Create new dialog</summary>
    /// <param name="pController">Controller reference</param>
    /// <returns>Window created</returns>
    /// <exception cref="std::exception">Creation failure</exception>
    static ConfigDialogView* createWindow(ConfigDialog* pController);

    /// <summary>Display window</summary>
    /// <exception cref="std::exception">No window or event exception</exception>
    void setVisible();
};

#define _CONFIG_DIALOG_VIEW_H_END
#include "config_dialog.h"

#endif
#endif
