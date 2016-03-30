/*******************************************************************************
    PANDORAGS project - PS1 GPU renderer
    ------------------------------------------------------------------------
    Author  :     Romain Vinders
    License :     GPLv2
    ------------------------------------------------------------------------
    File name :   config_manager.h
    Description : config management and singleton
*******************************************************************************/
#ifndef _CONFIG_MANAGER_H
#define _CONFIG_MANAGER_H
#include "config.h"


/* CONFIG MANAGER SINGLETON ------------------------------------------------- */
class ConfigManager
{
  protected:
    static Config _config;
    static int _currentProfile;  // 0 = default
    static int _gameAssociation; // 0 = none
    
  public:
    static Config* getInstance() // singleton pattern
    {
        return &_config;
    }
    static void init();
    
    // properties
    static void setConfig();
    static void unsetConfig();
    static void setProfile(int, bool);
    static void unsetProfile();
    static int  getGameProfile(char* gameExeName);
    static void setGameProfile(char* gameExeName);
};

#endif
