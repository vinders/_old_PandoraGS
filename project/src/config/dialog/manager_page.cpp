/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : tab page - profile manager
*******************************************************************************/
#include "../../globals.h"
#include <string>
#include <stdexcept>
#include <functional>
#include "../../lang/config_lang.h"
#include "../../res/resource.h"
#include "../../events/utils/logger.h"
#include "controls/common.h"
#include "controls/tab_page.h"
#include "controls/file_dialog.h"
#include "manager_page.h"
using namespace config::dialog;
using namespace config::dialog::controls;
using namespace std::literals::string_literals;


/*{
    FileDialog fileDialog(...);
    fileDialog.registerEvent(...onInit);
    fileDialog.registerEvent(...onCommand);
    fileDialog.registerEvent(...onConfirm);
}

onInit
{
    FileDialog::onInit(...); // call original handler
    //...
    // add other stuff here
    //...
}*/

//Button_SetState(GetDlgItem(hWindow,ID_DU_BOUTON), FALSE); // enlever focus de bouton quand relâché
