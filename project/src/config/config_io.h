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
        /// @param[out] pOutProfileNames  Profile name list to complete (or NULL to ignore it)
        static void loadConfig(std::vector<std::wstring>* pOutProfileNames = NULL);
        /// @brief Save config values
        /// @param[in] hasProfiles          Also save contained profiles (true) / only save main config (false)
        /// @param[in] isSavingAllProfiles  Save all profiles (true) or only profiles with update flag (false)
        /// @param[in] profiles             Array of config profiles (necessary, even if profiles not saved)
        /// @param[in] removedProfiles      List of all previous profiles with remove indicator
        /// @throws runtime_error  Saving failure
        static void saveConfig(const bool hasProfiles, const bool isSavingAllProfiles, 
                               const std::vector<ConfigProfile*>& profiles, const std::vector<bool>& removedProfiles);
        /// @brief Initialize list of profile names
        /// @param[in]  profileCount  Number of profiles to read
        /// @param[out] profileNames  Reference to list of names to complete
        static void listProfileNames(const uint32_t profileCount, std::vector<std::wstring>& profileNames);

        /// @brief Load profile values
        /// @param[in] id          Profile identifier
        /// @param[in] isNameRead  Read profile name (true) or ignore (false)
        /// @returns Allocated config profile container (with loaded values)
        static ConfigProfile* loadConfigProfile(const uint32_t id, const bool isNameRead = true);
        /// @brief Import profile values (external file)
        /// @param[in] id    New profile identifier
        /// @param[in] path  Import file path
        /// @returns Allocated config profile container (with loaded values)
        /// @throws invalid_argument  File not found
        static ConfigProfile* importConfigProfile(const uint32_t id, const std::wstring& path);
        /// @brief Read profile values
        /// @param[in]  reader   Data source reader
        /// @param[out] profile  Profile to complete
        template<typename T>
        static void readConfigProfileValues(ConfigFileIO<T>& reader, ConfigProfile& profile);

        /// @brief Save profile values
        /// @param[in] profile  Config profile container with values
        /// @throws runtime_error  Saving failure
        static void saveConfigProfile(ConfigProfile& profile);
        /// @brief Export profile values (external file)
        /// @param[in] profile  Config profile container with values
        /// @param[in] path     Export file path
        /// @throws runtime_error  Export failure
        static void exportConfigProfile(ConfigProfile& profile, const std::wstring& path);
        /// @brief Write profile values
        /// @param[in] writer   Data destination manager
        /// @param[in] profile  Source profile
        template<typename T>
        static void writeConfigProfileValues(ConfigFileIO<T>& writer, ConfigProfile& profile);

        /// @brief Remove profile (won't change associations !)
        /// @param[in] id  Profile identifier
        /// @throws runtime_error  Saving failure
        static void removeConfigProfile(const uint32_t id); // !!! change profile associations BEFORE calling this !!!

        /// @brief Remember a game/profile association (ingame)
        /// @param[in] profileId         Profile ID to associate with game
        /// @param[in] gameExecutableId  Game identifier
        static void setGameAssocation(const uint32_t profileId, const std::string& gameExecutableId);
        /// @brief Get the profile ID associated with a game (ingame)
        /// @param[in] gameExecutableId  Game identifier
        /// @returns Associated profile ID (or 0)
        static uint32_t getGameAssociation(const std::string& gameExecutableId);

        /// @brief Set the game/profile associations (settings)
        /// @param[in] associations  List of associated games/profiles
        static void setProfileAssociations(const std::vector<game_profile_association_t>& associations);
        /// @brief Get the list of all the game/profile associations (settings)
        /// @param[out] outAssociations  Destination variable for returned list
        static void getProfileAssociations(std::vector<game_profile_association_t>& outAssociations);
    };
}
