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
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onInit);
    TabPage::registerEvent(dialog_event_t::init, eventHandler);
    eventHandler.handler = std::function<DIALOG_EVENT_RETURN(PAGE_EVENT_HANDLER_ARGUMENTS)>(onNotify);
    TabPage::registerEvent(dialog_event_t::notify, eventHandler);
}



// -- event handlers -- --------------------------------------------

/// @brief Initialization event handler
DIALOG_EVENT_RETURN ProfilePage::onInit(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    ProfilePage& parent = getEventTargetPageReference(ProfilePage);
    parent.m_hPage = getEventWindowHandle();
    lang::ConfigLang& langRes = parent.getParentDialog<ConfigDialog>()->getLanguageResource();

    // insert tabs for sub-pages
    parent.m_activeSubPage = 0;
    std::vector<std::wstring> tabNames;
    tabNames.push_back(langRes.profile.filteringTab);
    tabNames.push_back(langRes.profile.screenTab);
    tabNames.push_back(langRes.profile.compatibilityTab);
    SimpleTabs::insertTabs(getEventWindowHandle(), IDC_PROFILE_TABS, tabNames, parent.m_activeSubPage);
    // create sub-pages
    for (uint32_t i = 0; i < parent.m_subPages.size(); ++i)
    {
        parent.m_subPages.at(i)->create(getEventWindowHandle(), 0, true);
        parent.m_subPages.at(i)->setVisible(i == parent.m_activeSubPage);
    }
    return DIALOG_EVENT_RETURN_VALID;
}


/// @brief Notification event handler
DIALOG_EVENT_RETURN ProfilePage::onNotify(PAGE_EVENT_HANDLER_ARGUMENTS)
{
    ProfilePage& parent = getEventTargetPageReference(ProfilePage);

    // sub-tab change
    if (getTabEventActionType() == controls::SimpleTabs::event_t::tabChanged)
    {
        if (parent.m_subPages.empty() == false)
        {
            parent.m_subPages.at(parent.m_activeSubPage)->setVisible(false);
            parent.m_activeSubPage = SimpleTabs::getActiveTabIndex(getTabEventControlId());
            parent.m_subPages.at(parent.m_activeSubPage)->setVisible(true);
        }
    }
    return DIALOG_EVENT_RETURN_VALID;
}



// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool ProfilePage::onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    for (uint32_t i = 0; i < m_subPages.size(); ++i)
    {
        if (m_subPages.at(i)->onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS_VALUES) == false)
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
