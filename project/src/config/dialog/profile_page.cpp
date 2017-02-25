/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - profile settings
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <stdexcept>
#include <functional>
#include <vector>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/dialog.h"
#include "config_dialog.h"
#include "controls/tab_page.h"
#include "controls/simple_tabs.hpp"
#include "profile_filtering_page.h"
#include "profile_screen_page.h"
#include "profile_compatibility_page.h"
#include "profile_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Create tab page - profile settings
/// @param[in] instance       Current instance handle
/// @param[in] pParentDialog  Parent dialog reference
ProfilePage::ProfilePage(controls::library_instance_t instance, controls::Dialog* pParentDialog)
    : controls::TabPage(instance, pParentDialog, IDD_PROFILE_PAGE), m_activeSubPage(0)
{
    // sub-pages
    m_subPages.push_back(std::make_shared<ProfileFilteringPage>(instance, pParentDialog));
    m_subPages.push_back(std::make_shared<ProfileScreenPage>(instance, pParentDialog));
    m_subPages.push_back(std::make_shared<ProfileCompatibilityPage>(instance, pParentDialog));

    // set event handlers
    TabPage::event_handler_t eventHandler;
    eventHandler.handler = onInit;
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = onNotify;
    TabPage::registerEvent(dialog_event_t::notify, eventHandler);
}

/// @brief Close tab page control - overridable method
void ProfilePage::overridableClose()
{
    TabPage::unregisterEvent(dialog_event_t::notify);
    TabPage::unregisterEvent(dialog_event_t::init);

    for (uint32_t i = 0; i < m_subPages.size(); ++i)
        m_subPages.at(i)->close();
    m_subPages.clear();
    m_activeSubPage = 0;
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
EVENT_RETURN ProfilePage::onInit(TabPage::event_args_t args)
{
    ProfilePage& parent = args.getParent<ProfilePage>();
    parent.m_hPage = args.window;
    lang::ConfigLang& langRes = parent.getParentDialog<ConfigDialog>()->getLanguageResource();

    // insert tabs for sub-pages
    parent.m_activeSubPage = 0;
    std::vector<std::wstring> tabNames;
    tabNames.push_back(langRes.profile.filteringTab);
    tabNames.push_back(langRes.profile.screenTab);
    tabNames.push_back(langRes.profile.compatibilityTab);
    SimpleTabs::insertTabs(args.window, IDC_PROFILE_TABS, tabNames, parent.m_activeSubPage);
    // create sub-pages
    for (uint32_t i = 0; i < parent.m_subPages.size(); ++i)
    {
        parent.m_subPages.at(i)->create(args.window, 0, true);
        parent.m_subPages.at(i)->setVisible(i == parent.m_activeSubPage);
    }
    return EVENT_RETURN_VALID;
}


/// @brief Notification event handler
EVENT_RETURN ProfilePage::onNotify(TabPage::event_args_t args)
{
    ProfilePage& parent = args.getParent<ProfilePage>();

    // sub-tab change
    if (isTabEventAction(args.notifierData, controls::SimpleTabs::event_t::tabChanged))
    {
        if (parent.m_subPages.empty() == false)
        {
            parent.m_subPages.at(parent.m_activeSubPage)->setVisible(false);
            parent.m_activeSubPage = SimpleTabs::getActiveTabIndex(tabControlId(args.notifierData));
            parent.m_subPages.at(parent.m_activeSubPage)->setVisible(true);
        }
    }
    return EVENT_RETURN_VALID;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool ProfilePage::onDialogConfirm(Dialog::event_args_t& args)
{
    for (uint32_t i = 0; i < m_subPages.size(); ++i)
    {
        if (m_subPages.at(i)->onDialogConfirm(args) == false)
            return false;
    }
    return true;
}

/// @brief Language change event
/// @param[in] isRecursive    Also translate controls in child pages or not
void ProfilePage::onLanguageChange(const bool isRecursive)
{
    window_handle_t hPage = getPageHandle();
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    // update tabs for sub-pages
    std::vector<std::wstring> tabNames;
    tabNames.push_back(langRes.profile.filteringTab);
    tabNames.push_back(langRes.profile.screenTab);
    tabNames.push_back(langRes.profile.compatibilityTab);
    SimpleTabs::setTabTitles(hPage, IDC_PROFILE_TABS, tabNames);

    // sub-pages
    m_subPages.at(m_activeSubPage)->setVisible(false);
    if (isRecursive)
    {
        for (uint32_t i = 0; i < m_subPages.size(); ++i)
        {
            m_subPages.at(i)->onLanguageChange(true);
        }
    }
    m_subPages.at(m_activeSubPage)->setVisible(true);
}
