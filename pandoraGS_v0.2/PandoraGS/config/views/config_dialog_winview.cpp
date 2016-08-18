/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_dialog_winview.cpp
Description : configuration dialog - view
*******************************************************************************/
#include "globals.h"
#if _DIALOGAPI == DIALOGAPI_WIN32
using namespace std;
#include "config_dialog_winview.h"


/// <summary>Create dialog view container</summary>
/// <param name="pController">Controller reference</param>
ConfigDialogView::ConfigDialogView(ConfigDialog* pController)
{
    m_pPages = NULL;
    m_pController = pController;
}
/// <summary>Destroy dialog view container</summary>
ConfigDialogView::~ConfigDialogView()
{
    if (m_pPages != NULL)
        delete[] m_pPages; //sub-pages items are deleted by sub-controllers
    m_pPages = NULL;
}

/// <summary>Create new dialog</summary>
/// <param name="pController">Controller reference</param>
/// <returns>Window created</returns>
/// <exception cref="std::exception">Creation failure</exception>
ConfigDialogView* ConfigDialogView::createWindow(ConfigDialog* pController)
{
    // create main window
    ConfigDialogView* pWindow = new ConfigDialogView(pController);

    // get sub-pages from sub-controllers
    pWindow->m_pPages = new ConfigPageView*[CONFIG_DIALOG_PAGES_NB];
    for (int p = 0; p < CONFIG_DIALOG_PAGES_NB; ++p)
        pWindow->m_pPages[p] = pController->getConfigPage(p)->getView();
}

/// <summary>Display window</summary>
/// <exception cref="std::exception">No window or event exception</exception>
void ConfigDialogView::setVisible()
{

}

#endif
