/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
File name :   config_io.h
Description : configuration IO toolbox (load/save)
*******************************************************************************/
#ifndef _CONFIG_IO_H
#define _CONFIG_IO_H
#include "globals.h"

#include <string>
#include "config.h"
typedef ConfigProfile* CfgProfilePtr;

// game/profile association
typedef struct CONFIGIO_GAMEPROFILE
{
    unsigned int profileId;
    std::string gameId;
    CONFIGIO_GAMEPROFILE* pNext;
} ConfigIO_GameProfile_t;


// Configuration IO toolbox
class ConfigIO
{
private:
    ConfigIO() {} // no instance allowed

public:
    /// <summary>Load config values from registry/file</summary>
    /// <param name="pConfig">Existing config container to fill</param>
    /// <param name="hasProfileArray">Alloc an empty array with the appropriate size</param>
    /// <param name="hasProfileValues">Fill the array with profile containers</param>
    static void loadConfig(Config* pConfig, bool hasProfileArray, bool hasProfileValues);
    /// <summary>Save config values to registry/file</summary>
    /// <param name="pConfig">Config container with values</param>
    /// <param name="hasProfiles">Also save contained profiles (true) or config alone (false)</param>
    /// <exception cref="std::exception">Key creation/opening failure</exception>
    static void saveConfig(Config* pConfig, bool hasProfiles);

    /// <summary>Load only frame limiting values</summary>
    /// <param name="pConfig">Existing config container to change</param>
    static void loadFrameLimitConfig(Config* pConfig);

    /// <summary>Load specific profile values from registry/file</summary>
    /// <param name="id">Profile identifier</param>
    /// <returns>Allocated config profile container (with loaded values)</returns>
    static ConfigProfile* loadConfigProfile(unsigned int id);
    /// <summary>Save profile values to registry/file</summary>
    /// <param name="pProfile">Config profile container with values</param>
    /// <exception cref="std::exception">Key creation/opening failure</exception>
    static void saveConfigProfile(ConfigProfile* pProfile);

    /// <summary>Remember a game/profile association (ingame)</summary>
    /// <param name="profileId">Profile ID to associate with game</param>
    /// <param name="gameId">Associated game ID</param>
    /// <exception cref="std::exception">Key creation/opening failure</exception>
    static void setGameAssocation(unsigned int profileId, std::string gameId);
    /// <summary>Get the ID of the profile associated with a game (ingame)</summary>
    /// <param name="gameId">Game ID</param>
    /// <returns>Associated profile ID (or 0)</returns>
    static unsigned int getGameAssociation(std::string gameId);

    /// <summary>Define all the games/profiles associations (settings)</summary>
    /// <param name="pAssociations">List of associated games/profiles (will be freed)</param>
    static void setProfileAssociations(ConfigIO_GameProfile_t* pAssociations);
    /// <summary>Get the list of all the games/profiles associations (settings)</summary>
    /// <param name="ppAssociations">Null pointer for returned list</param>
    static void getProfileAssociations(ConfigIO_GameProfile_t* pAssociations);
};

#endif
