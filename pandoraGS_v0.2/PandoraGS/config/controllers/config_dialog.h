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


// Configuration dialog box - business logic
class ConfigDialog
{
private:
    ConfigDialogView* m_pView; // visuals container
public:
    LanguageDialogResource* m_pLang; // language resource
    ConfigPage* m_pPages; // dialog pages


public:
    ConfigDialog();
    ~ConfigDialog();

    void showDialog();
};

#define _CONFIG_DIALOG_H_END

#endif
