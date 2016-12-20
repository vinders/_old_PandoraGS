/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_dialog.cpp
Description : configuration dialog - business logic
*******************************************************************************/
using namespace std;
#include "config_dialog.h"
#include "logger.h"

/// <summary>Initialize controller variables</summary>
ConfigDialog::ConfigDialog()
{
    m_pView = NULL;
    m_pLang = NULL;
    m_pPages = NULL;
}
/// <summary>Destroy controller data</summary>
ConfigDialog::~ConfigDialog()
{
    if (m_pView != NULL)
        delete m_pView;
    m_pView = NULL;
    if (m_pLang != NULL)
        delete m_pLang;
    m_pLang = NULL;
    if (m_pPages != NULL)
    {
        for (int p = 0; p < CONFIG_DIALOG_PAGES_NB; ++p)
        {
            if (m_pPages[p] != NULL)
                delete m_pPages[p];
        }
        delete [] m_pPages;
    }
    m_pPages = NULL;
    if (m_pProfileNames != NULL)
        delete [] m_pProfileNames;
    m_pProfileNames = NULL;
}

/// <summary>Create and display config dialog</summary>
void ConfigDialog::showDialog()
{
    try
    {
        // read config and default profile (other profiles loaded when needed)
        Config::init(); // default config
        ConfigIO::loadConfig(true, false); // user config
        Config::useProfile(0);
        m_pProfileNames = ConfigIO::listProfiles();

        // load language resources
        m_pLang = new LanguageDialogResource();
        m_pLang->setLanguage((langcode_t)Config::gen_langCode);

        // create sub-controllers with their views
        m_pPages = new ConfigPage*[CONFIG_DIALOG_PAGES_NB];
        m_pPages[CONFIG_PAGE_GENERAL] = new ConfigPageGeneral(this);
        m_pPages[CONFIG_PAGE_MANAGER] = new ConfigPageManager(this);
        m_pPages[CONFIG_PAGE_PROFILE] = new ConfigPageProfile(this);
        for (int p = 0; p < CONFIG_DIALOG_PAGES_NB; ++p)
            m_pPages[p]->createView();

        // create main view
        m_pView = ConfigDialogView::createWindow(this);
        // display dialog
        m_pView->setVisible(); // display + event handling loop
    }
    catch (std::exception exc)
    {
        Logger::getInstance()->writeErrorEntry("ConfigDialog.showDialog", exc.what()); 
    }
}

/// <summary>Save current config</summary>
/// <returns>Success</returns>
bool ConfigDialog::saveConfig()
{
    //...
    //...
    //...
    //ConfigIO::saveConfig(true);
    return true;
}

/// <summary>Change language</summary>
/// <param name="code">Language code</param>
/// <returns>Changes indicator</returns>
bool ConfigDialog::setLanguage(int code)
{
    switch (Config::gen_langCode)
    {
        case Langcode_english:
        case Langcode_spanish:
        case Langcode_french:
        case Langcode_german:
        case Langcode_customFile: m_pLang->setLanguage((langcode_t)code); break;
        default: return false; break;
    }
    return true;
}

