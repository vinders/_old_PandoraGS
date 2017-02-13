/*******************************************************************************
PANDORAGS project - PS1 GPU driver
------------------------------------------------------------------------
Author  :     Romain Vinders
License :     GPLv2
------------------------------------------------------------------------
Description : file selection dialog control
*******************************************************************************/
#pragma once

#include "../../../globals.h"
#include <cstdint>
#include <string>
#include "common.h"
#include "dialog.h"
using namespace std::literals::string_literals;

/// @namespace config
/// Configuration management
namespace config
{
    /// @namespace config.dialog
    /// Configuration dialog management
    namespace dialog
    {
        /// @namespace config.dialog.controls
        /// Dialog controls
        namespace controls
        {
            /// @class FileDialog
            /// @brief File selection dialog control
            /// @see Dialog
            class FileDialog : protected Dialog
            {
            public:
                /// @enum file_mode_t
                /// @brief File selection mode
                enum class file_mode_t : int32_t
                {
                    load = 0,
                    save = 1
                };


            private:
                file_mode_t m_mode;       ///< File selection mode
                int32_t m_fieldId;        ///< File selection field identifier
                int32_t m_browserId;      ///< File browser identifier
                std::wstring m_filePath;  ///< Selected file path


            public:
                /// @brief Create dialog box
                /// @param[in] instance  Current instance handle
                /// @param[in] mode      File selection mode
                /// @throws invalid_argument  Invalid instance
                FileDialog(library_instance_t instance, file_mode_t mode);
                /// @brief Destroy dialog box
                virtual ~FileDialog() {}

                /// @brief Get selected file path
                /// @returns Selected file path
                inline std::wstring getFilePath() const noexcept
                {
                    return m_filePath;
                }

                /// @brief Display modal dialog box
                /// @param[in] dialogResourceId   Dialog description identifier
                /// @param[in] fieldResourceId    File path field identifier
                /// @param[in] hParentWindow      Parent window handle
                /// @param[in] browserResourceId  Browse button identifier
                /// @param[in] defaultFile        Default file path
                /// @returns Dialog result
                /// @throws runtime_error  Dialog creation error or runtime error
                Dialog::result_t showDialog(const int32_t dialogResourceId, const int32_t fieldResourceId, window_handle_t hParentWindow, 
                                            const int32_t browserResourceId, const std::wstring& defaultFile = L""s);

            public:
                /// @brief Dialog initialization event handler
                static DIALOG_EVENT_RETURN onInit(DIALOG_EVENT_HANDLER_ARGUMENTS);
                /// @brief Dialog command event handler
                static DIALOG_EVENT_RETURN onCommand(DIALOG_EVENT_HANDLER_ARGUMENTS);
                /// @brief Dialog confirm event handler - check validity
                static DIALOG_EVENT_RETURN onConfirm(DIALOG_EVENT_HANDLER_ARGUMENTS);
            };
        }
    }
}