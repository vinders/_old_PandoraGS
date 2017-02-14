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
#include "controls/tab_page.h"
#include "profile_filtering_page.h"
#include "profile_screen_page.h"
#include "profile_compatibility_page.h"
#include "profile_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Language change event
/// @param[in] isRecursive  Also translate controls in child pages or not
void ProfilePage::onLanguageChange(bool isRecursive)
{
    window_handle_t pPage = getPageHandle();
    //...

    if (isRecursive)
    {
        //...
    }
}

//Button_SetState(GetDlgItem(hWindow,ID_DU_BOUTON), FALSE); // enlever focus de bouton quand relâché
