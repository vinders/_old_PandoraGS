/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   config_manager.cpp
    Description : config management and singleton
*******************************************************************************/
using namespace std;
#include "../globals.h"
#include "config_manager.h"

/* static variables */
Config ConfigManager::_config;
int ConfigManager::_currentProfile  = DEFAULT_CONFIG;
int ConfigManager::_gameAssociation = NO_CONFIG;

/* initialize singleton ----------------------------------------------------- */
void ConfigManager::init()
{
    _currentProfile  = DEFAULT_CONFIG;
    _gameAssociation = NO_CONFIG;
}

/* define singleton target -------------------------------------------------- */
void ConfigManager::setConfig()
{
    _config.loadConfigValues();
    _currentProfile  = DEFAULT_CONFIG;
    _gameAssociation = NO_CONFIG;
}

/* remove singleton target -------------------------------------------------- */
void ConfigManager::unsetConfig()
{
    _currentProfile  = DEFAULT_CONFIG;
    _gameAssociation = NO_CONFIG;
}

/* define new config profile ------------------------------------------------ */
void ConfigManager::setProfile(int profile, bool first)
{
    _config.setProfile(profile, false);
    
    _currentProfile  = profile;
    if (first)
        _gameAssociation = profile;
}

/* remove config profile ---------------------------------------------------- */
void ConfigManager::unsetProfile()
{
    _config.unsetProfile(false);
    _currentProfile  = DEFAULT_CONFIG;
    _gameAssociation = NO_CONFIG;
}


// GETTERS/SETTERS
//______________________________________________________________________________

/* retrieve game profile association ---------------------------------------- */
int ConfigManager::getGameProfile(char* gameExeName)
{
    if (_gameAssociation == NO_CONFIG && gameExeName)
    {
        //!lire association
            //!si trouvée, _gameAssociation = assoc;
    }
    return _gameAssociation;
}

/* update game profile association ------------------------------------------ */
void ConfigManager::setGameProfile(char* gameExeName)
{
    if (gameExeName && _gameAssociation != _currentProfile)
    {
        if (_currentProfile > DEFAULT_CONFIG)
        {
            //!créer/modifier association
        }
        else
        {
            //!effacer association
        }
    }
}
