/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : configuration input/output toolbox
*******************************************************************************/
#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <list>
#include "config_file_io.h"
#include "config_profile.h"
#include "config.h"

/// @namespace config
/// Configuration management
namespace config
{
    typedef ConfigProfile* ConfigProfilePointer; ///< Pointer to profile - type for arrays

    /// @struct game_profile_association_t
    /// @brief Game / profile association container
    struct game_profile_association_t
    {
        uint32_t profileId;           ///< Config profile ID
        std::string gameExecutableId; ///< Game identifier
    };


    /// @class ConfigIO
    /// @brief Configuration input/output toolbox
    class ConfigIO
    {
    public:
        /// @brief Load configuration values
        /// @param pOutProfileNames Profile name list to complete (or NULL to ignore it)
        static void loadConfig(std::vector<std::wstring>* pOutProfileNames = NULL);
        /// @brief Save config values
        /// @param hasProfiles Also save contained profiles (true) / only save main config (false)
        /// @param profiles Array of config profiles (necessary, even if profiles not saved)
        /// @throw Saving failure
        static void saveConfig(bool hasProfiles, std::vector<ConfigProfile*>& profiles);
        /// @brief Initialize list of profile names
        /// @param profileCount Number of profiles to read
        /// @param profileNames Reference to list of names to complete
        static void listProfileNames(uint32_t profileCount, std::vector<std::wstring>& profileNames);

        /// @brief Load profile values
        /// @param id Profile identifier
        /// @param isNameRead Read profile name (true) or ignore (false)
        /// @return Allocated config profile container (with loaded values)
        /// @throw Memory allocation failure
        static ConfigProfile* loadConfigProfile(uint32_t id, bool isNameRead = false);
        /// @brief Import profile values (external file)
        /// @param id New profile identifier
        /// @param path Import file path
        /// @return Allocated config profile container (with loaded values)
        /// @throw Memory allocation failure
        static ConfigProfile* importConfigProfile(uint32_t id, std::wstring& path);
        /// @brief Read profile values
        /// @param reader Data source reader
        /// @param profile Profile to complete
        template<typename T>
        static void readConfigProfileValues(ConfigFileIO<T>& reader, ConfigProfile& profile);

        /// @brief Save profile values
        /// @param profile Config profile container with values
        /// @throw Saving failure
        static void saveConfigProfile(ConfigProfile& profile);
        /// @brief Export profile values (external file)
        /// @param profile Config profile container with values
        /// @param path Export file path
        /// @throw Export failure
        static void exportConfigProfile(ConfigProfile& profile, std::wstring& path);
        /// @brief Write profile values
        /// @param writer Data destination manager
        /// @param profile Source profile
        template<typename T>
        static void writeConfigProfileValues(ConfigFileIO<T>& writer, ConfigProfile& profile);

        /// @brief Remove profile (won't change associations !)
        /// @param id Profile identifier
        /// @throw Saving failure
        static void removeConfigProfile(uint32_t id); // !!! change profile associations BEFORE calling this !!!

        /// @brief Remember a game/profile association (ingame)
        /// @param profileId Profile ID to associate with game
        /// @param gameExecutableId Game identifier
        /// @throw Saving failure
        static void setGameAssocation(uint32_t profileId, std::string& gameExecutableId);
        /// @brief Get the profile ID associated with a game (ingame)
        /// @param gameExecutableId Game identifier
        /// @return Associated profile ID (or 0)
        static uint32_t getGameAssociation(std::string& gameExecutableId);

        /// @brief Set the game/profile associations (settings)
        /// @param associations List of associated games/profiles
        static void setProfileAssociations(std::list<game_profile_association_t>& associations);
        /// @brief Get the list of all the game/profile associations (settings)
        /// @param outAssociations Destination variable for returned list
        static void getProfileAssociations(std::list<game_profile_association_t>& outAssociations);
    };
}
