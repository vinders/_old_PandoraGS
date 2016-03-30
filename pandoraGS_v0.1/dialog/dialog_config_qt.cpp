/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   dialog_config_qt.cpp
    Description : configuration management QT dialog - main
*******************************************************************************/
#define _INCLUDE_DIALOGAPI
#include "../globals.h"
#if _SYSTEM != _WINDOWS || _DIALOGAPI == _QTAPI
using namespace std;
#include <stdio.h>
#include <string.h>
#include "dialog_config.h"

    
/* dialog constructor ------------------------------------------------------- */
DialogConfig::DialogConfig()
{
    // get config and language content
    ConfigManager::setConfig(ALL_CONFIGS);
    Config* globalConfig = ConfigManager::getInstance();
    _langContent = LangConfig::getInstance(globalConfig->getLangCode());
  
}

/* dialog destructor -------------------------------------------------------- */
DialogConfig::~DialogConfig()
{

    // remove config and language content
    ConfigManager::unsetConfig();
    if (_langContent)
        delete _langContent;
    _langContent = NULL;
}


/* save current config ------------------------------------------------------ */
long DialogConfig::saveConfig()
{
    long result = 0;
    Config* globalConfig = ConfigManager::getInstance();
    //!copier la valeur de chaque item dans la config
    //!result = sauvegarder(...)
    return result;
}


/* display and manage dialog box -------------------------------------------- */
long DialogConfig::showDialog()
{
    long response = 0;

    
    return response;
}


/* display warning message box ---------------------------------------------- */
void DialogConfig::showWarning(const char* origin, const char* message)
{
    // create final message
    int bufflength = 10 + strlen(origin) + strlen(message);
    char* buffer = new char[bufflength + 1];
    buffer[bufflength] = '\0';
    if (snprintf(buffer, bufflength, "Error(%s) : %s", origin, message) < 0)
    {
        delete buffer;
        bufflength = 27;
        buffer = new char[28];
        buffer[27] = '\0';
        strcpy(buffer, "Error : dialog system error");
    }
    
    // display message box

    delete [] buffer;
}



#endif
