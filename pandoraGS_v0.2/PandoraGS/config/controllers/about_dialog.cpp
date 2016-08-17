/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   about_dialog.cpp
Description : about dialog - business logic
*******************************************************************************/
#include <string>
using namespace std;
#include "about_dialog.h"
#include "log_utility.h"

/// <summary>Initialize "about" view</summary>
/// <exception cref="std::exception">Allocation failure</exception>
AboutDialog::AboutDialog()
{
    m_pView = NULL;
}
/// <summary>Destroy "about" view</summary>
AboutDialog::~AboutDialog()
{
    if (m_pView != NULL)
        delete m_pView;
    m_pView = NULL;
}

/// <summary>Open "about" dialog</summary>
void AboutDialog::showDialog()
{
    try
    {
        // create "about" window with caption, title and content
        m_pView = AboutDialogView::createWindow(PLUGIN_ABOUT_TITLE);

        // display window
        m_pView->setVisible(); // display + event handling loop
    }
    catch (std::exception exc)
    {
        try { LogUtility::getInstance()->writeErrorEntry("AboutDialog.showDialog", exc.what()); }
        catch (...) {}
    }
}
