/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - general settings
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <stdexcept>
#include <functional>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/text_field.hpp"
#include "controls/combo_box.h"
#include "controls/check_box.hpp"
#include "controls/group_box.h"
#include "controls/label.hpp"
#include "config_dialog.h"
#include "controls/tab_page.h"
#include "general_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/// @brief Language change event
/// @param[in] isRecursive    Also translate controls in child pages or not
void GeneralPage::onLanguageChange(const bool isRecursive)
{
    window_handle_t hPage = getPageHandle();
    lang::ConfigLang& langRes = getParentDialog<ConfigDialog>()->getLanguageResource();

    GroupBox::setText(hPage, IDS_GEN_GROUP1, langRes.generalSettings.groupDisplay);
    //...
    GroupBox::setText(hPage, IDS_GEN_GROUP2, langRes.generalSettings.groupTimer);
    //...
}

//Button_SetState(GetDlgItem(hWindow,ID_DU_BOUTON), FALSE); // enlever focus de bouton quand relâché
