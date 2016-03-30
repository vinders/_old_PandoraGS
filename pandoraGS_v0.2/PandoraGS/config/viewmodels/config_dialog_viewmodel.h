/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_dialog_viewmodel.h
Description : configuration dialog - business logic
*******************************************************************************/
#ifndef _CONFIG_DIALOG_VIEWMODEL_H
#define _CONFIG_DIALOG_VIEWMODEL_H
#include "globals.h"

#include "lang.h"
#include "config_io.h"
#include "config_page_viewmodel.h"
#include "config_pgeneral_viewmodel.h"
#include "config_pmanager_viewmodel.h"
#include "config_pprofile_viewmodel.h"
#include "config_dialog_winview.h"
#include "config_dialog_qtview.h"


// Configuration dialog box - business logic
class ConfigDialogVM
{
private:
    ConfigDialogView* m_pView; // visuals container
public:
    LanguageDialogResource* m_pLang; // language resource
    ConfigPageVM* m_pPages; // dialog pages
    Config* m_pConfig; // managed configuration container


public:
    ConfigDialogVM();
    ~ConfigDialogVM();

    void showDialog();
};

#endif
