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
#include <array>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "config_dialog.h"
#include "controls/tab_page.h"
#include "profile_filtering_page.h"
#include "profile_screen_page.h"
#include "profile_compatibility_page.h"
#include "profile_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


// -- specialized handlers -- --------------------------------------

/// @brief Language change event
/// @returns Validity
bool ProfilePage::onDialogConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS)
{
    //...copy config settings
    //...
}

/// @brief Language change event
/// @param[in] isRecursive    Also translate controls in child pages or not
void ProfilePage::onLanguageChange(const bool isRecursive)
{
    window_handle_t hPage = getPageHandle();
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    //...tabs

    if (isRecursive)
    {
        //...sub-pages
    }
}

//Button_SetState(GetDlgItem(hWindow,ID_DU_BOUTON), FALSE); // enlever focus de bouton quand relâché
